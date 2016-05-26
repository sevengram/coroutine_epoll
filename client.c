#include "csapp.h"

void *func(void *index);

int main(int argc, char **argv)
{
    int thread_num = atoi(argv[1]);
    int repeat_times = atoi(argv[2]);

    pthread_t tid[thread_num];
    int i;
    for (i = 0; i < thread_num; ++i) {
        pthread_create(&tid[i], NULL, func, (void *) repeat_times);
    }
    for (i = 0; i < thread_num; ++i) {
        pthread_join(tid[i], NULL);
    }
    printf("all done!!!\n");
    return 0;
}

void *func(void *arg)
{
    int n = (int) arg;
    char rbuf[5];
    const char *msg = "test";
    int clientfd = Open_clientfd("127.0.0.1", 12400);
    int i;
    for (i = 0; i < n; ++i) {
        Rio_writen(clientfd, msg, 5);
        Rio_readn(clientfd, rbuf, 5);
    }
    Close(clientfd);
    return NULL;
}
