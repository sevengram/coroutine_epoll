#include "csapp.h"

#define THREAD_NUMS 1000

void *func(void *index);

int main()
{
    pthread_t tid[THREAD_NUMS];
    int i;
    for (i = 0; i < THREAD_NUMS; ++i) {
        pthread_create(&tid[i], NULL, func, (void *) i);
    }
    for (i = 0; i < THREAD_NUMS; ++i) {
        pthread_join(tid[i], NULL);
    }
    printf("all done!!!\n");
    return 0;
}

void *func(void *index)
{
    char rbuf[5];
    const char *msg = "test";
    int clientfd = Open_clientfd("127.0.0.1", 12400);

    int i;
    for (i = 0; i < 1000; ++i) {
        Rio_writen(clientfd, msg, 5);
        Rio_readn(clientfd, rbuf, 5);
    }
    printf("%d task done!\n", (int) index);
    Close(clientfd);
    return NULL;
}
