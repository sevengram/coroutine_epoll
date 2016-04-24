#ifndef __FIBER_SEMAPHORE_H
#define __FIBER_SEMAPHORE_H

#include "coroutine.h"

typedef struct
{
    volatile uint64_t value;
    task_t **wait_queue;
} co_sem_t;

int co_sem_init(co_sem_t *f_sem, int value);

int co_sem_destroy(co_sem_t *f_sem);

int co_sem_post(co_sem_t *f_sem);

int co_sem_wait(task_t *task, co_sem_t *f_sem);

int co_sem_getvalue(co_sem_t *f_sem, int *sval);

#endif
