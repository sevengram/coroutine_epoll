#ifndef __FIBER_H
#define __FIBER_H

#include <stdint.h>
#include <semaphore.h>
#include <ucontext.h>

#define STACK_SIZE    (4096*32)
#define MAX_EVENT_SIZE  (1024*64)

enum TASK_STATUS
{
    READY = 0,
    RUNABLE = 1,
    RUNNING = 2,
    SUSPEND = 3,
    DEAD = 4,
};

typedef struct task
{
    void (*entry)(struct task *);

    void (*func)(struct task *, void *);

    void *arg;
    int status;
    char stack[STACK_SIZE];
    ucontext_t ctx;
    struct schedule *sch;
} task_t;

typedef struct schedule
{
    int stop;
    sem_t sem_done;
    sem_t sem_free;
    sem_t sem_used;
    volatile size_t head;
    size_t tail;
    size_t size;
    task_t **tasks;
    task_t *running;
    struct pool *mpool;
    pthread_t tid;
    ucontext_t mctx;
} schedule_t;

typedef struct pool
{
    int stop;
    size_t size;
    size_t index;
    int epoll_fd;
    schedule_t **threads;
    task_t **blocked_io_set;
} pool_t;

pool_t *pool_create(int thread_num);

void pool_free(pool_t *pl);

void pool_start(pool_t *pl);

void pool_stop(pool_t *pl);

task_t *task_create(void (*func)(task_t *, void *), void *arg);

int task_add(pool_t *pl, task_t *tsk);

int task_suspend(task_t *tsk);

int task_yield(task_t *tsk);

int task_resume(task_t *task);

int fd_suspend(task_t *task, int fd);

int fd_wake(pool_t *pl, int fd);

#endif
