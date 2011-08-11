#include "xt/net/server/socket.h"

#define XT_NET_SERVER_SOCKET_LISTEN_MAX_PENDING 8

int xt_net_server_socket_accept(int serversocket,
    struct sockaddr_in *client_address,
    socklen_t *client_address_size)
{
  int client_socket;

  *client_address_size = sizeof *client_address;
  client_socket = accept(serversocket, (struct sockaddr *) client_address,
      client_address_size);

  return client_socket;
}

int xt_net_server_socket_create(unsigned short port)
{
  int serversocket;
  struct sockaddr_in server_address;

  serversocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serversocket >= 0) {
    fcntl(serversocket, F_SETFL, O_NONBLOCK);
    memset(&server_address, '\0', sizeof server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
    if (bind(serversocket, (struct sockaddr *) &server_address,
            sizeof server_address) >= 0) {
      if (listen(serversocket, XT_NET_SERVER_SOCKET_LISTEN_MAX_PENDING) < 0) {
        close(serversocket);
        serversocket = -1;
      }
    } else {
      close(serversocket);
      serversocket = -1;
    }
  }

  return serversocket;
}

void xt_net_server_socket_destroy(int serversocket)
{
  if (serversocket != -1) {
    close(serversocket);
  }
}
