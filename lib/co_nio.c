#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include "co_nio.h"

int fdnoblock(int fd)
{
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

ssize_t co_write(task_t *task, int fd, const char *buf, size_t nbyte)
{
    while (1) {
        ssize_t write_byte = write(fd, buf, nbyte);
        if (write_byte >= 0) {
            return write_byte;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            suspend_fd(task, fd);
            int epoll_fd = task->sch->mpool->epoll_fd;
            struct epoll_event ev;
            ev.events = EPOLLOUT | EPOLLONESHOT | EPOLLET;
            ev.data.fd = fd;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
                return -1;
            }
            swapcontext(&task->ctx, &task->sch->mctx);
        } else {
            return -1;
        }
    }
}

ssize_t co_read(task_t *task, int fd, char *buf, size_t nbyte)
{
    while (1) {
        ssize_t read_byte = read(fd, buf, nbyte);
        if (read_byte >= 0) {
            return read_byte;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            suspend_fd(task, fd);

            int epoll_fd = task->sch->mpool->epoll_fd;
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLONESHOT | EPOLLET;
            ev.data.fd = fd;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
                return -1;
            }
            swapcontext(&task->ctx, &task->sch->mctx);
        } else {
            return -1;
        }
    }
}

ssize_t co_writen(task_t *task, int fd, const char *buf, size_t nbyte)
{
    size_t rbyte = nbyte;
    const char *tbuf = buf;
    while (rbyte) {
        ssize_t rval = co_write(task, fd, tbuf, rbyte);
        if (rval <= 0) {
            return (nbyte - rbyte);
        }
        rbyte -= rval;
        tbuf += rval;
    }
    return nbyte;
}

ssize_t co_readn(task_t *task, int fd, char *buf, size_t nbyte)
{
    size_t rbyte = nbyte;
    char *tbuf = buf;
    while (rbyte) {
        ssize_t rval = co_read(task, fd, tbuf, rbyte);
        if (rval <= 0) {
            return (nbyte - rbyte);
        }
        rbyte -= rval;
        tbuf += rval;
    }
    return nbyte;
}

ssize_t co_send(task_t *task, int fd, const char *buf, size_t len, int flags)
{
    while (1) {
        ssize_t send_byte = send(fd, buf, len, flags);
        if (send_byte >= 0) {
            return send_byte;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            suspend_fd(task, fd);

            int epoll_fd = task->sch->mpool->epoll_fd;
            struct epoll_event ev;
            ev.events = EPOLLOUT | EPOLLONESHOT | EPOLLET;
            ev.data.fd = fd;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
                return -1;
            }
            swapcontext(&task->ctx, &task->sch->mctx);
        } else {
            return -1;
        }
    }
}

ssize_t co_recv(task_t *task, int fd, char *buf, size_t len, int flags)
{
    while (1) {
        ssize_t recv_byte = recv(fd, buf, len, flags);
        if (recv_byte >= 0) {
            return recv_byte;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            suspend_fd(task, fd);

            int epoll_fd = task->sch->mpool->epoll_fd;
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLONESHOT | EPOLLET;
            ev.data.fd = fd;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
                return -1;
            }
            swapcontext(&task->ctx, &task->sch->mctx);
        } else {
            return -1;
        }
    }
}

ssize_t co_sendto(task_t *task, int sockfd, const char *buf, size_t len, int flags,
                  struct sockaddr *dest_addr, socklen_t addrlen)
{
    while (1) {
        ssize_t send_byte = sendto(sockfd, buf, len, flags, dest_addr, addrlen);
        if (send_byte >= 0) {
            return send_byte;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            suspend_fd(task, sockfd);

            int epoll_fd = task->sch->mpool->epoll_fd;
            struct epoll_event ev;
            ev.events = EPOLLOUT | EPOLLONESHOT | EPOLLET;
            ev.data.fd = sockfd;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
                return -1;
            }
            swapcontext(&task->ctx, &task->sch->mctx);
        } else {
            return -1;
        }
    }
}

ssize_t co_recvfrom(task_t *task, int sockfd, char *buf, size_t len, int flags,
                    struct sockaddr *src_addr, socklen_t *addrlen)
{
    while (1) {
        ssize_t recv_byte = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
        if (recv_byte >= 0) {
            return recv_byte;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            suspend_fd(task, sockfd);

            int epoll_fd = task->sch->mpool->epoll_fd;
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLONESHOT | EPOLLET;
            ev.data.fd = sockfd;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
                return -1;
            }
            swapcontext(&task->ctx, &task->sch->mctx);
        } else {
            return -1;
        }
    }
}
