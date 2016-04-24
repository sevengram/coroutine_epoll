#include <stdio.h>
#include "co_mutex.h"

co_mutex_t mtx;

int g_val_array[8];

void func(task_t *fiber, void *data);

int main()
{
    co_mutex_init(&mtx);
    pool_t *pl = create_pool(4);
    open_pool(pl);

    int i;
    for (i = 0; i < 1024; ++i) {
        add_task(pl, create_task(func, NULL));
    }

    close_pool(pl);
    free_pool(pl);
    co_mutex_destroy(&mtx);
}

void func(task_t *fiber, void *data)
{
    int i, j;
    for (i = 0; i < 64; ++i) {
        co_mutex_lock(fiber, &mtx);
        for (j = 0; j < 8; ++j) {
            printf("%d ", ++g_val_array[j]);
        }
        printf("\n");
        co_mutex_unlock(&mtx);
    }
}
