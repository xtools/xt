#ifndef xt_net_socket_h
#define xt_net_socket_h

ssize_t xt_net_socket_receive(int client_socket, void *data, size_t data_size);

ssize_t xt_net_socket_send(int client_socket, void *data, size_t data_size);

#endif
