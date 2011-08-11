#include "xt/core/standard.h"
#include "xt/net/client/socket.h"

int xt_net_client_socket_create(char *server_ip_address,
    unsigned short server_port)
{
  int clientsocket;
  struct sockaddr_in server_address;

  memset(&server_address, '\0', sizeof server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(server_ip_address);
  server_address.sin_port = htons(server_port);

  clientsocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientsocket >= 0) {
    if (connect(clientsocket, (struct sockaddr *) &server_address,
            sizeof server_address) >= 0) {
      fcntl(clientsocket, F_SETFL, O_NONBLOCK);
    } else {
      close(clientsocket);
      clientsocket = -1;
    }
  }

  return clientsocket;
}

void xt_net_client_socket_destroy(int clientsocket)
{
  if (clientsocket != -1) {
    close(clientsocket);
  }
}
