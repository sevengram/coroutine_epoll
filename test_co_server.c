#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/times.h>
#include "tcp_server.h"
#include "co_nio.h"

int n = 0;

struct tms tmsstart, tmsend;

clock_t start, end;

int request_count = 0;

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
    if (k == request_count - 1) {
        long tps;
        double rt, ut, st;
        end = times(&tmsend);
        tps = sysconf(_SC_CLK_TCK);
        rt = (end - start) / (double) tps;
        ut = (tmsend.tms_utime - tmsstart.tms_utime) / (double) tps;
        st = (tmsend.tms_stime - tmsstart.tms_stime) / (double) tps;
        printf("%.5f, %.5f, %.5f\n", rt, ut, st);
        fflush(stdout);
        n = 0;
    }
    close(fd);
    free(arg);
    free(rbuf);
}

int main(int argc, char **argv)
{
    int thread_num = atoi(argv[1]);
    request_count = atoi(argv[2]);
    const char *server_ip = "127.0.0.1";
    pool_t *pl = create_pool(thread_num);
    tcp_server *server = create_tcp_server(server_ip, 12400, handle);
    open_pool(pl);
    run_tcp_server(pl, server);
    free_tcp_server(server);
    close_pool(pl);
    return 0;
}

