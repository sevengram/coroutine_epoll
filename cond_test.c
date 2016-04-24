#include <stdio.h>
#include "co_cond.h"

co_mutex_t mtx;
co_cond_t cond;

int i = 0;

void func1(task_t *task, void *data);

void func2(task_t *task, void *data);

int main()
{
    co_mutex_init(&mtx);
    co_cond_init(&cond);

    pool_t *pl = create_pool(1);
    open_pool(pl);
    add_task(pl, create_task(func1, NULL));
    add_task(pl, create_task(func2, NULL));
    close_pool(pl);
    free_pool(pl);

    co_cond_destroy(&cond);
    co_mutex_destroy(&mtx);
}

void func1(task_t *task, void *data)
{
    for (i = 1; i <= 6; i++) {
        co_mutex_lock(task, &mtx);
        printf("thread1: lock %d\n", __LINE__);
        if (i % 3 == 0) {
            printf("thread1:signal 1  %d\n", __LINE__);
            co_cond_signal(&cond);
            printf("thread1:signal 2  %d\n", __LINE__);
        }
        co_mutex_unlock(&mtx);
        printf("thread1: unlock %d\n\n", __LINE__);
        yield_task(task);
    }
}

void func2(task_t *task, void *data)
{
    while (i < 6) {
        co_mutex_lock(task, &mtx);
        printf("thread2: lock %d\n", __LINE__);
        if (i % 3 != 0) {
            printf("thread2: wait 1  %d\n", __LINE__);
            co_cond_wait(task, &cond, &mtx);
            printf("thread2: wait 2  %d\n", __LINE__);
        }
        co_mutex_unlock(&mtx);
        printf("thread2: unlock %d\n\n", __LINE__);
        yield_task(task);
    }
}
