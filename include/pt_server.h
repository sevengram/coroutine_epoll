#ifndef __PT_SERVER_H
#define __PT_SERVER_H

typedef struct
{
    int socket;

    void *(*handle)(void *);
} pt_server;

pt_server *create_pt_server(const char *ip, int port, void *(*handle)(void *));

void free_pt_server(pt_server *server);

void run_pt_server(pt_server *server);


#endif
