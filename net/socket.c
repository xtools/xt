#include "xt/core/tools.h"
#include "xt/net/socket.h"

ssize_t xt_net_socket_receive(int socket, void *data, size_t data_size)
{
  return recv(socket, data, data_size, 0);
}

ssize_t xt_net_socket_send(int socket, void *data, size_t data_size)
{
  return send(socket, data, data_size, 0);
}
