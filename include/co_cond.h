#ifndef __FIBER_COND_H
#define __FIBER_COND_H

#include "co_mutex.h"

typedef struct
{
    volatile uint64_t value;
    task_t **wait_queue;
} co_cond_t;

int co_cond_init(co_cond_t *cond);

int co_cond_destroy(co_cond_t *cond);

int co_cond_signal(co_cond_t *cond);

int co_cond_broadcast(co_cond_t *cond);

int co_cond_wait(task_t *task, co_cond_t *cond, co_mutex_t *mtx);

#endif
