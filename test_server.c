#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "coroutine.h"
#include "tcp_server1.h"
#include <stdio.h>
#include <pthread.h>

void* data_processor(schedule_t* fiber, void *data);

int main() {
    const char *server_ip = "127.0.0.1";
    schedule_t *sch = schedule_create();
    struct tcp_server *server = create_tcp_server(server_ip, 12400, data_processor);
    pthread_t pid;
    pthread_create(&pid, NULL, schedule_thread, sch);
    run_tcp_server(sch, server);

    return 0;
}

void* data_processor(schedule_t* fiber, void *data) {
    printf("hello!!\n");
//    int i = 0;
//    int fd = *(int*)data;
//    char *rbuf = malloc(10);
//    char sbuf[5] = "fine";
//    char *tbuf;
//    int tlen, wrlen;
//
//    while (1) {
//        tlen = 10;
//        tbuf = rbuf;
//        memset(rbuf, 0, 10);
//        while (tlen > 0) {
//            int wrlen = rivus_read(fiber, fd, tbuf, tlen);
//            if (wrlen == 0) {
//                close(fd);
//                return 0;
//            }
//            tlen -= wrlen;
//            tbuf += wrlen;
//        }
//
//        tlen = 5;
//        tbuf = sbuf;
//        while (tlen > 0) {
//            int wrlen = rivus_write(fiber, fd, tbuf, tlen);
//            if (wrlen < 0) {
//                close(fd);
//                return 0;
//            }
//            tlen -= wrlen;
//            tbuf += wrlen;
//        }
//        ++i;
//    }
//    free(rbuf);
//    return 0;
}