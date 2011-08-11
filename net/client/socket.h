#ifndef xt_net_client_socket_h
#define xt_net_client_socket_h

int xt_net_client_socket_create(char *server_ip_address,
    unsigned short server_port);

void xt_net_client_socket_destroy(int clientsocket);

#endif
