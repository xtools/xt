#ifndef xt_net_server_socket_h
#define xt_net_server_socket_h

#include "xt/core/standard.h"

int xt_net_server_socket_accept(int serversocket,
    struct sockaddr_in *client_address, socklen_t *client_address_size);

int xt_net_server_socket_create(unsigned short port);

void xt_net_server_socket_destroy(int serversocket);

#endif
