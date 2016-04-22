#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H

#include "coroutine.h"

struct TcpServer
{
    int socket;

    void(*handle)(task_t *, void *);
};

struct TcpServer *create_tcp_server(const char *ip, int port, void(*handle)(task_t *, void *));

void free_tcp_server(struct TcpServer *server);

void run_tcp_server(pool_t *sch, struct TcpServer *server);


#endif
