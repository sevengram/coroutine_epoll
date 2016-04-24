#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/times.h>
#include "tcp_server.h"
#include "co_nio.h"

int n = 0;

struct tms tmsstart, tmsend;

clock_t start, end;

void handle(task_t *task, void *arg)
{
    int k = (int) __sync_fetch_and_add(&n, 1);
    if (k == 0) {
        start = times(&tmsstart);
    }
    int fd = *(int *) arg;
    char *rbuf = malloc(5);
    const char *msg = "fine";
    while (co_readn(task, fd, rbuf, 5) != 0) {
        co_writen(task, fd, msg, 5);
    }
    if (k == 999) {
        long tps;
        end = times(&tmsend);
        tps = sysconf(_SC_CLK_TCK);
        printf("real:%.3f\n", (end - start) / (double) tps);
        printf("user:%.3f\n", (tmsend.tms_utime - tmsstart.tms_utime) / (double) tps);
        printf("system:%.3f\n", (tmsend.tms_stime - tmsstart.tms_stime) / (double) tps);
        n = 0;
    }
    close(fd);
    free(rbuf);
}

int main()
{
    const char *server_ip = "127.0.0.1";
    pool_t *pl = create_pool(2);
    tcp_server *server = create_tcp_server(server_ip, 12400, handle);
    open_pool(pl);
    run_tcp_server(pl, server);
    free_tcp_server(server);
    close_pool(pl);
    return 0;
}

