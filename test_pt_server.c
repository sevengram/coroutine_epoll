#include <stdlib.h>
#include <sys/times.h>
#include "pt_server.h"
#include "csapp.h"

int n = 0;

struct tms tmsstart, tmsend;

clock_t start, end;

void *handle(void *arg)
{
    int k = (int) __sync_fetch_and_add(&n, 1);
    if (k == 0) {
        start = times(&tmsstart);
    }
    int fd = *(int *) arg;
    char *rbuf = malloc(5);
    const char *msg = "fine";
    while (Rio_readn(fd, rbuf, 5) != 0) {
        Rio_writen(fd, msg, 5);
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
    return 0;
}

int main()
{
    const char *server_ip = "127.0.0.1";
    pt_server *server = create_pt_server(server_ip, 12400, handle);
    run_pt_server(server);
    free_pt_server(server);
    return 0;
}
