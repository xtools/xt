#include "xt/core/log.h"
#include "xt/config/tools.h"
#include "xt/case/list.h"
#include "xt/core/tools.h"
#include "xt/net/engine/iengine.h"
#include "xt/net/http/message.h"
#include "xt/net/http/post.h"
#include "xt/net/post/system.h"
#include "xt/net/server/system.h"
#include "xt/net/test/engine.h"
#include "xt/net/test/http/engine.h"
#include "xt/net/test/message.h"

#define TEST_SERVER_MAX_THREADS 32
#define TEST_HTTP_SERVER_MAX_THREADS 32

struct xt_core_test_server_context_t {
  void *server_wide_shared_database;
};
typedef struct xt_core_test_server_context_t xt_core_test_server_context_t;

int main(int argc, char *argv[])
{
  xt_config_disable_if_running_batch_tests(argc, argv);

  xt_core_test_server_context_t custom_server_context;

  xt_net_server_system_t *server;
  unsigned short port;
  xt_net_engine_iengine_t iengine;
  xt_core_imessage_t messagey;
  xt_net_post_ipost_t ipost;

  xt_net_server_system_t *http_server;
  unsigned short http_port;
  xt_net_engine_iengine_t http_iengine;
  xt_core_imessage_t http_messagey;
  xt_net_post_ipost_t http_ipost;

  xt_core_log_t *log;

  log = xt_core_log_create(stdout);
  if (!log) {
    xt_core_trace_exit("x_audit_log_create");
  }

  port = 20000;

  xt_net_engine_iengine_init(&iengine, xt_net_test_engine_create,
      xt_net_test_engine_destroy, xt_net_test_engine_get_handler_for_message,
      xt_net_test_engine_maintain, xt_net_test_engine_run,
      xt_net_test_engine_start, xt_net_test_engine_stop);

  xt_core_imessage_init(&messagey, xt_core_message_destroy,
      xt_core_message_get_client_socket, xt_core_message_get_engine_id,
      xt_core_message_get_type);

  xt_net_post_ipost_init(&ipost, xt_net_post_system_compare, xt_net_post_system_create,
      xt_net_post_system_create_decoy, xt_net_post_system_destroy, xt_net_post_system_destroy_decoy,
      xt_net_post_system_get_last_receive_activity_time, xt_net_post_system_get_socket,
      xt_net_post_system_get_stats, xt_net_post_system_receive_message,
      xt_net_post_system_receive_messages, xt_net_post_system_send_message,
      xt_net_post_system_send_messages, xt_net_post_system_is_socket_closed,
      xt_net_post_system_mod, xt_net_post_system_compare_equal);

  server = xt_net_server_system_create("standard", port, port, TEST_SERVER_MAX_THREADS,
      &messagey, &ipost, XT_NET_ENGINE_NO_GET_NAME_FUNCTION,
      XT_NET_SERVER_SYSTEM_NO_CONFIG_SYSTEM, log);
  assert(server);

  if (!xt_net_server_system_register_engine(server, XT_NET_ENGINE_TEST,
          &custom_server_context, &iengine, 2, 2, XT_NET_MAINTAIN_CONSTANTLY,
          XT_NET_TEST_MESSAGE_TYPE_COUNT)) {
    xt_core_trace_exit("x_net_server_register_engine");
  }

  http_port = 50000;

  xt_net_engine_iengine_init(&http_iengine, xt_net_test_http_engine_create,
      xt_net_test_http_engine_destroy,
      xt_net_test_http_engine_get_handler_for_message,
      xt_net_test_http_engine_maintain, xt_net_test_http_engine_run,
      xt_net_test_http_engine_start, xt_net_test_http_engine_stop);

  xt_core_imessage_init(&http_messagey, xt_net_http_message_destroy,
      xt_net_http_message_get_client_socket, xt_net_http_message_get_engine_id,
      xt_net_http_message_get_type);

  xt_net_post_ipost_init(&http_ipost, xt_net_http_post_compare,
      xt_net_http_post_create, xt_net_http_post_create_decoy,
      xt_net_http_post_destroy, xt_net_http_post_destroy_decoy,
      xt_net_http_post_get_last_receive_activity_time,
      xt_net_http_post_get_socket, xt_net_http_post_get_stats,
      xt_net_http_post_receive_message, xt_net_http_post_receive_messages,
      xt_net_http_post_send_message, xt_net_http_post_send_messages,
      xt_net_http_post_is_socket_closed, xt_net_http_post_mod,
      xt_net_http_post_compare_equal);

  http_server = xt_net_server_system_create("http", http_port, http_port,
      TEST_HTTP_SERVER_MAX_THREADS, &http_messagey, &http_ipost,
      XT_NET_ENGINE_NO_GET_NAME_FUNCTION, XT_NET_SERVER_SYSTEM_NO_CONFIG_SYSTEM, log);
  assert(http_server);

  if (!xt_net_server_system_start(server)) {
    xt_core_trace_exit("x_net_server_start");
  }

  if (!xt_net_server_system_start(http_server)) {
    xt_core_trace_exit("x_net_server_start");
  }

  xt_net_server_system_destroy(server);
  xt_net_server_system_destroy(http_server);
  xt_core_log_destroy(log);

  return 0;
}
