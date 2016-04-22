#include <stdlib.h>
#include "conio.h"
#include "tcp_server.h"

void handle(task_t *task, void *arg)
{
    int fd = *(int *) arg;
    char *rbuf = malloc(5);
    const char *msg = "fine";
    while (co_readn(task, fd, rbuf, 5) != 0) {
        co_writen(task, fd, msg, 5);
    }
    free(rbuf);
}

int main()
{
    const char *server_ip = "127.0.0.1";
    pool_t *sch = pool_create(8);
    tcp_server *server = create_tcp_server(server_ip, 12400, handle);
    pool_start(sch);
    run_tcp_server(sch, server);
    return 0;
}

