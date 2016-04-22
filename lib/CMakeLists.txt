set(LIBTASK_SRC coroutine.c conio.c tcp_server.c)
add_library(coronio STATIC ${LIBTASK_SRC})
