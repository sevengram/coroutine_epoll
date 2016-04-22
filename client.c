#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define THREAD_NUMS 1000

void *func(void *index);

int cfd = 0;
int ftd = 0;

int main()
{
    time_t ts, te;
    int i = 0;
    ts = time(&ts);
    pthread_t tid[THREAD_NUMS];
    for (; i < THREAD_NUMS; ++i) {
        if (pthread_create(&tid[i], NULL, func, (void *) i) == 0) {
            ++cfd;
        }
    }

    void *retval;
    i = 0;
    for (; i < THREAD_NUMS; ++i) {
        pthread_join(tid[i], &retval);
    }
    te = time(&te);
    printf("all done!!!\n");
    printf("spend time: %d seconds\n", te - ts);
    return 0;
}

void *func(void *index)
{
    const char *server_ip = "127.0.0.1";
    const int server_port = 12400;
    struct sockaddr_in server_address;
    char sbuf[10] = "testfiber";
    char rbuf[5];
    int client_socket;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket failure!\n");
        return NULL;
    }

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    server_address.sin_port = htons(server_port);

    if (connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        printf("connect server failure!\n");
        return NULL;
    }
    printf("client socket: %d\n", client_socket);

    int i = 0;
    for (; i < 100; ++i) {
        ssize_t rwbyte;
        size_t tbyte = 10;
        char *tbuf = sbuf;
        while (tbyte > 0) {
            rwbyte = write(client_socket, tbuf, tbyte);
            if (rwbyte > 0) {
                tbyte -= rwbyte;
                tbuf += rwbyte;
            }
        }
        tbyte = 5;
        tbuf = rbuf;
        memset(rbuf, 0, 5);
        while (tbyte > 0) {
            rwbyte = read(client_socket, rbuf, tbyte);
            if (rwbyte > 0) {
                tbyte -= rwbyte;
                tbuf += rwbyte;
            }
        }
    }
    close(client_socket);
    ++ftd;
    printf("%d task done!\n", ftd);
    printf("%d thread done!\n", cfd);
    return NULL;
}