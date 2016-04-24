#ifndef __FIBER_MUTEX_H
#define __FIBER_MUTEX_H

#include "coroutine.h"

typedef struct
{
    volatile uint64_t value;
    task_t **wait_queue;
} co_mutex_t;

int co_mutex_init(co_mutex_t *mtx);

int co_mutex_destroy(co_mutex_t *mtx);

int co_mutex_lock(task_t *fiber, co_mutex_t *mtx);

int co_mutex_unlock(co_mutex_t *mtx);

#endif
