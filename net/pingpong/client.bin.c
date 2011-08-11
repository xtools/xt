#include "xt/core/message.h"
#include "xt/core/tools.h"
#include "xt/net/client/system.h"
#include "xt/net/pingpong/engine.h"
#include "xt/net/pingpong/message.h"
#include "xt/net/pingpong/ping.h"

#define PING_CLIENT_SLEEP_MICROSECONDS XT_CORE_STANDARD_SLEEP_MICROSECONDS
#define PING_CLIENT_SEND_MESSAGE_COUNT 8

struct ping_client_context_t {
  xt_core_bool_t pong_received;
  unsigned short pongs_received;
};
typedef struct ping_client_context_t ping_client_context_t;

static void handle_pong(void *custom_client_context,
    xt_core_message_t *message);
static void print_uinferno_exit();

void handle_pong(void *custom_client_context, xt_core_message_t *message)
{
  assert(custom_client_context);
  assert(message);
  ping_client_context_t *client_context;
  char *pong_string;

  client_context = custom_client_context;

  client_context->pong_received = xt_core_bool_true;
  client_context->pongs_received++;
  pong_string = xt_core_message_take_string(message);
  printf("[ping] server sent back \"%s\"\n", pong_string);

  free(pong_string);
}

void print_uinferno_exit()
{
  printf("\n    uinferno: ping_client serverIp serverPort pingMessage\n\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  xt_net_client_system_t *client;
  xt_core_message_t *ping_message;
  ping_client_context_t ping_client_context;
  xt_pingpong_ping_t *ping;
  xt_core_log_t *log;

  unsigned short server_port;
  char *server_ip;
  char *ping_string;

  unsigned short message_count;

  signal(SIGPIPE, SIG_IGN);

  if (argc < 4) {
    print_uinferno_exit();
  }
  server_ip = argv[1];
  server_port = atoi(argv[2]);
  ping_string = argv[3];

  ping_client_context.pong_received = xt_core_bool_false;
  ping_client_context.pongs_received = 0;

  log = xt_core_log_create(stdout);
  if (!log) {
    xt_core_trace_exit("x_audit_log_create");
  }

  client = xt_net_client_system_create(server_ip, server_port, server_port,
      XT_NET_ENGINE_NO_GET_NAME_FUNCTION, &ping_client_context, log);
  if (!client) {
    xt_core_trace_exit("x_net_client_create");
  }

  if (!xt_net_client_system_register_engine(client, XT_NET_ENGINE_PING,
          xt_PINGPONG_MESSAGE_TYPE_COUNT)) {
    xt_core_trace_exit("x_net_client_register_engine");
  }

  xt_net_client_system_register_message_handler(client, XT_NET_ENGINE_PING,
      xt_PINGPONG_MESSAGE_PONG, handle_pong);

  for (message_count = 0; message_count < PING_CLIENT_SEND_MESSAGE_COUNT;
       message_count++) {

    ping = xt_pingpong_ping_create(ping_string);
    if (ping) {
      ping_message = xt_pingpong_message_create
        (XT_CORE_IMESSAGE_NO_CLIENT_SOCKET, xt_PINGPONG_MESSAGE_PING);
      if (ping_message) {
        if (xt_pingpong_ping_add_to_message(ping, ping_message)) {
          if (!xt_net_client_system_send_message(client, ping_message)) {
            xt_core_trace_exit("x_net_client_send_message() failed");
          }
        } else {
          xt_core_trace("x_pingpong_ping_add_to_message");
          xt_core_message_destroy(ping_message);
        }
      } else {
        xt_core_trace("x_pingpong_message_create");
      }
      xt_pingpong_ping_destroy(ping);
    } else {
      xt_core_trace_exit("x_pingpong_ping_create");
    }
  }

  do {
    xt_net_client_system_process_messages(client);
    usleep(PING_CLIENT_SLEEP_MICROSECONDS);
  } while (ping_client_context.pongs_received
      < PING_CLIENT_SEND_MESSAGE_COUNT);

  xt_net_client_system_destroy(client);
  xt_core_log_destroy(log);

  return 0;
}
