set(LIBTASK_SRC
        coroutine.c
        co_nio.c
        tcp_server.c
        pt_server.c
        co_semaphore.c
        co_mutex.c
        co_cond.c)
add_library(coronio STATIC ${LIBTASK_SRC})
