#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "coroutine.h"

#define TASK_LIMIT   (1024*64)
#define THREAD_LIMIT    256


pool_t *pool_create(int thread_nums)
{
    if (thread_nums <= 0 || thread_nums > THREAD_LIMIT) {
        return NULL;
    }
    uint32_t tnums = 1;
    while (thread_nums > tnums) {
        tnums *= 2;
    }
    pool_t *pl = calloc(1, sizeof(pool_t));
    pl->size = tnums;
    pl->threads = calloc(tnums, sizeof(schedule_t *));
    pl->blocked_io_set = calloc(TASK_LIMIT, sizeof(task_t *));

    int i;
    uint32_t qsize = TASK_LIMIT / tnums;
    schedule_t *sch;
    for (i = 0; i < tnums; ++i) {
        sch = calloc(1, sizeof(schedule_t));
        sch->size = qsize;
        sch->tasks = calloc(qsize, sizeof(task_t *));
        sem_init(&sch->sem_used, 0, 0);
        sem_init(&sch->sem_free, 0, qsize);
        pl->threads[i] = sch;
        sch->mpool = pl;
    }
    return pl;
}

void pool_free(pool_t *pool)
{
    schedule_t *sch;
    int i;
    for (i = 0; i < pool->size; ++i) {
        sch = pool->threads[i];
        sem_destroy(&sch->sem_free);
        sem_destroy(&sch->sem_used);
        free(sch->tasks);
        free(sch);
    }
    free(pool->threads);
    free(pool->blocked_io_set);
    free(pool);
}

static
void *schedule_run(void *arg)
{
    schedule_t *sch = (schedule_t *) arg;
    task_t *task;

    while (1) {
        sem_wait(&sch->sem_used);
        while (sch->tasks[sch->tail] == NULL) {
            usleep(1);
        }
        while (sch->tasks[sch->tail]) {
            task = sch->tasks[sch->tail];
            switch (task->status) {
                case READY:
                    getcontext(&task->ctx);
                    task->ctx.uc_stack.ss_sp = task->stack;
                    task->ctx.uc_stack.ss_size = STACK_SIZE;
                    makecontext(&task->ctx, (void (*)(void)) task->entry, 1, task);
                case RUNABLE:
                    task->status = RUNNING;
                    sch->running = task;
                    swapcontext(&sch->mctx, &task->ctx);
                    break;
                default:
                    break;
            }
            if (task->status == DEAD) {
                free(task);
            }
        }
        sch->tail = (sch->tail + 1) & (sch->size - 1);
    }
}

void pool_start(pool_t *pl)
{
    int i;
    for (i = 0; i < pl->size; ++i) {
        pthread_create(&pl->threads[i]->tid, NULL, schedule_run, (void *) pl->threads[i]);
    }
}

void pool_stop(pool_t *pl)
{
    pl->stop = 1;

    int i;
    for (i = 0; i < pl->size; ++i) {
        pl->threads[i]->stop = 1;
    }

    for (i = 0; i < pl->size; ++i) {
        int sval;
        schedule_t *sch = pl->threads[i];
        sem_getvalue(&sch->sem_free, &sval);
        if (sval != sch->size) {
            sem_wait(&sch->sem_done);
        }
    }

    for (i = 0; i < pl->size; ++i) {
        pthread_cancel(pl->threads[i]->tid);
        pthread_join(pl->threads[i]->tid, NULL);
    }
}

int task_detach(task_t *task)
{
    if (task == NULL) {
        return -1;
    }
    schedule_t *sch = task->sch;
    sch->tasks[sch->tail] = NULL;
    sem_post(&sch->sem_free);
    return 0;
}

static
void task_entry(task_t *task)
{
    task->func(task, task->arg);
    task->status = DEAD;
    task_detach(task);
    schedule_t *sch = task->sch;
    if (sch->stop) {
        int sval;
        sem_getvalue(&sch->sem_free, &sval);
        if (sval == sch->size) {
            sem_post(&sch->sem_done);
        }
    }
    swapcontext(&task->ctx, &sch->mctx);
}

task_t *task_create(void (*func)(task_t *, void *), void *arg)
{
    task_t *ptask = calloc(1, sizeof(task_t));
    ptask->entry = task_entry;
    ptask->func = func;
    ptask->arg = arg;
    ptask->status = READY;
    return ptask;
}

int task_add(pool_t *pool, task_t *task)
{
    if (pool == NULL || task == NULL || pool->stop) {
        return -1;
    }

    schedule_t *sch = pool->threads[pool->index];
    while (sem_trywait(&sch->sem_free) != 0) {
        pool->index = (pool->index + 1) & (pool->size - 1);
        sch = pool->threads[pool->index];
    }

    task->sch = sch;
    int index = (int) (__sync_fetch_and_add(&sch->head, 1) & (sch->size - 1));
    sch->tasks[index] = task;
    sem_post(&sch->sem_used);
    pool->index = (pool->index + 1) & (pool->size - 1);
    return 0;
}

int task_yield(task_t *task)
{
    if (task == NULL) {
        return -1;
    }
    task->status = RUNABLE;
    schedule_t *sch = task->sch;
    sch->tasks[sch->tail] = NULL;
    int index = (int) (__sync_fetch_and_add(&sch->head, 1) & (sch->size - 1));
    sch->tasks[index] = task;
    sem_post(&sch->sem_used);
    swapcontext(&task->ctx, &sch->mctx);
    return 0;
}


int task_suspend(task_t *task)
{
    if (task == NULL) {
        return -1;
    }
    task->status = SUSPEND;
    schedule_t *sch = task->sch;
    sch->tasks[sch->tail] = NULL;
    return 0;
}

int fd_suspend(task_t *task, int fd)
{
    if (task == NULL || fd < 0) {
        return -1;
    }
    task_suspend(task);
    task->sch->mpool->blocked_io_set[fd] = task;
    return 0;
}

int task_resume(task_t *task)
{
    if (task == NULL) {
        return -1;
    }
    task->status = RUNABLE;
    schedule_t *sch = task->sch;
    int index = (int) (__sync_fetch_and_add(&sch->head, 1) & (sch->size - 1));
    sch->tasks[index] = task;
    sem_post(&sch->sem_used);
    return 0;
}

int fd_wake(pool_t *pool, int fd)
{
    if (pool == NULL || fd < 0) {
        return -1;
    }
    return task_resume(pool->blocked_io_set[fd]);
}