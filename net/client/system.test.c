#include "xt/config/tools.h"
#include "xt/core/message.h"
#include "xt/core/tools.h"
#include "xt/net/client/system.h"
#include "xt/net/ping.h"
#include "xt/net/test/engine.h"
#include "xt/net/test/message.h"

#define XT_NET_CLIENT_SYSTEM_TEST_SLEEP_MICROSECONDS XT_CORE_STANDARD_SLEEP_MICROSECONDS

struct test_client_context_t {
  xt_core_bool_t pong_received;
};
typedef struct test_client_context_t test_client_context_t;

static void handle_pong(void *custom_client_context,
    xt_core_message_t *message);

void handle_pong(void *custom_client_context, xt_core_message_t *message)
{
  assert(custom_client_context);
  assert(message);
  test_client_context_t *client_context;
  char *pong_string;

  client_context = custom_client_context;

  client_context->pong_received = xt_core_bool_true;
  pong_string = xt_core_message_take_string(message);
  printf("%s\n", pong_string);

  free(pong_string);
  assert(client_context->pong_received);
}

int main(int argc, char *argv[])
{
  xt_config_disable_if_running_batch_tests(argc, argv);

  xt_net_client_system_t *client;
  xt_core_message_t *ping_message;
  test_client_context_t test_client_context;
  xt_net_ping_t *ping;
  xt_core_log_t *log;

  test_client_context.pong_received = xt_core_bool_false;

  log = xt_core_log_create(stdout);
  if (!log) {
    xt_core_trace_exit("x_audit_log_create");
  }

  client = xt_net_client_system_create("127.0.0.1", 20000, 20000,
      XT_NET_ENGINE_NO_GET_NAME_FUNCTION, &test_client_context, log);
  if (!client) {
    xt_core_trace_exit("x_net_client_create()");
  }
  xt_net_client_system_register_engine(client, XT_NET_ENGINE_TEST,
      XT_NET_TEST_MESSAGE_TYPE_COUNT);
  xt_net_client_system_register_message_handler(client, XT_NET_ENGINE_TEST,
      XT_NET_TEST_MESSAGE_PONG, handle_pong);

  ping = xt_net_ping_create("penta");
  if (!ping) {
    xt_core_trace_exit("x_net_ping_create");
  }

  ping_message = xt_net_test_message_create
    (XT_CORE_IMESSAGE_NO_CLIENT_SOCKET, XT_NET_TEST_MESSAGE_PING);
  if (!ping_message) {
    xt_core_trace_exit("x_net_testmessage_create");
  }

  if (!xt_net_ping_add_to_message(ping, ping_message)) {
    xt_core_trace_exit("x_net_ping_add_to_message");
  }

  xt_net_ping_destroy(ping);

  if (xt_net_client_system_send_message(client, ping_message)) {
    printf("client sent ping message\n");
  } else {
    xt_core_message_destroy(ping_message);
    xt_core_trace_exit("x_net_client_send_message");
  }

  do {
    xt_net_client_system_process_messages(client);
    usleep(XT_NET_CLIENT_SYSTEM_TEST_SLEEP_MICROSECONDS);
  } while (!test_client_context.pong_received);

  xt_net_client_system_destroy(client);
  xt_core_log_destroy(log);

  return 0;
}
