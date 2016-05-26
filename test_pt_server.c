#include <stdlib.h>
#include <sys/times.h>
#include "pt_server.h"
#include "csapp.h"

int n = 0;

struct tms tmsstart, tmsend;

clock_t start, end;

int request_count = 0;

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
    free(rbuf);
    return 0;
}

int main(int argc, char **argv)
{
    request_count = atoi(argv[1]);
    const char *server_ip = "127.0.0.1";
    pt_server *server = create_pt_server(server_ip, 12400, handle);
    run_pt_server(server);
    free_pt_server(server);
    return 0;
}
