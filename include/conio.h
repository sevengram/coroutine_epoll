#ifndef __SYNC_IO_H
#define __SYNC_IO_H

#include <sys/socket.h>
#include "coroutine.h"

int fdnoblock(int fd);

ssize_t co_write(task_t *fiber, int fd, const char *buf, size_t nbyte);

ssize_t co_read(task_t *fiber, int fd, char *buf, size_t nbyte);

ssize_t co_writen(task_t *fiber, int fd, const char *buf, size_t nbyte);

ssize_t co_readn(task_t *fiber, int fd, char *buf, size_t nbyte);

ssize_t co_send(task_t *fiber, int sockfd, const char *buf, size_t len, int flags);

ssize_t co_recv(task_t *fiber, int sockfd, char *buf, size_t len, int flags);

ssize_t co_sendto(task_t *fiber, int sockfd, const char *buf, size_t len, int flags,
                  struct sockaddr *dest_addr, socklen_t addrlen);

ssize_t co_recvfrom(task_t *fiber, int sockfd, char *buf, size_t len, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen);

#endif
