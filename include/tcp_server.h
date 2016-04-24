#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H

#include "coroutine.h"

typedef struct
{
    int socket;

    void (*handle)(task_t *, void *);
} tcp_server;

tcp_server *create_tcp_server(const char *ip, int port, void(*handle)(task_t *, void *));

void free_tcp_server(tcp_server *server);

void run_tcp_server(pool_t *sch, tcp_server *server);


#endif
