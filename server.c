#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "conio.h"
#include "tcp_server.h"

void data_processor(task_t *fiber, void *data);

int main()
{
    const char *server_ip = "127.0.0.1";
    pool_t *sch = pool_create(8);
    struct TcpServer *server = create_tcp_server(server_ip, 12400, data_processor);
    pool_start(sch);
    run_tcp_server(sch, server);
    return 0;
}

void data_processor(task_t *fiber, void *data)
{
    int i = 0;
    int fd = *(int *) data;
    char *rbuf = malloc(10);
    char sbuf[5] = "fine";
    char *tbuf;
    size_t tlen;

    while (1) {
        tlen = 10;
        tbuf = rbuf;
        memset(rbuf, 0, 10);
        while (tlen > 0) {
            ssize_t wrlen = co_read(fiber, fd, tbuf, tlen);
            if (wrlen == 0) {
                close(fd);
                return;
            }
            tlen -= wrlen;
            tbuf += wrlen;
        }
        tlen = 5;
        tbuf = sbuf;
        while (tlen > 0) {
            ssize_t wrlen = co_write(fiber, fd, tbuf, tlen);
            if (wrlen < 0) {
                close(fd);
                return;
            }
            tlen -= wrlen;
            tbuf += wrlen;
        }
        ++i;
    }
    free(rbuf);
}
