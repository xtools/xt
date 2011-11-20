#include "xt/case/list.h"
#include "xt/core/imessage.h"
#include "xt/core/log.h"
#include "xt/core/tools.h"
#include "xt/net/engine/iengine.h"
#include "xt/net/pingpong/engine.h"
#include "xt/net/pingpong/message.h"
#include "xt/net/post/ipost.h"
#include "xt/net/post/system.h"
#include "xt/net/server/system.h"

#define PING_SERVER_MAX_THREADS 32

struct ping_server_context_t {
  void *server_wide_shared_database;
};
typedef struct ping_server_context_t ping_server_context_t;

static void print_use_exit();

void print_use_exit()
{
  printf("\n    use: ping_server server_port\n\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  xt_net_server_system_t *server;
  ping_server_context_t custom_server_context;
  unsigned short server_port;
  xt_net_engine_iengine_t iengine;
  xt_core_imessage_t messagey;
  xt_net_post_ipost_t ipost;
  xt_core_log_t *log;

  if (argc < 2) {
    print_use_exit();
  }
  server_port = atoi(argv[1]);

  log = xt_core_log_create(stdout);
  if (!log) {
    xt_core_trace_exit("x_audit_log_create");
  }

  xt_net_engine_iengine_init(&iengine, xt_pingpong_engine_create,
      xt_pingpong_engine_destroy, xt_pingpong_engine_get_handler_for_message,
      xt_pingpong_engine_maintain, xt_pingpong_engine_run,
      xt_pingpong_engine_start, xt_pingpong_engine_stop);

  xt_core_imessage_init(&messagey, xt_core_message_destroy,
      xt_core_message_get_client_socket, xt_core_message_get_engine_id,
      xt_core_message_get_type);

  xt_net_post_ipost_init(&ipost, xt_net_post_system_compare,
      xt_net_post_system_create, xt_net_post_system_create_decoy,
      xt_net_post_system_destroy, xt_net_post_system_destroy_decoy,
      xt_net_post_system_get_last_receive_activity_time,
      xt_net_post_system_get_socket, xt_net_post_system_get_stats,
      xt_net_post_system_receive_message, xt_net_post_system_receive_messages,
      xt_net_post_system_send_message, xt_net_post_system_send_messages,
      xt_net_post_system_is_socket_closed, xt_net_post_system_mod,
      xt_net_post_system_compare_equal);

  server = xt_net_server_system_create("ping", server_port, server_port,
      PING_SERVER_MAX_THREADS, &messagey, &ipost,
      XT_NET_ENGINE_NO_GET_NAME_FUNCTION,
      XT_NET_SERVER_SYSTEM_NO_CONFIG_SYSTEM, log);
  if (!server) {
    xt_core_trace_exit("x_net_server_create");
  }

  if (!xt_net_server_system_register_engine(server, XT_NET_ENGINE_PING,
          &custom_server_context, &iengine, 2, 2, XT_NET_MAINTAIN_1_MINUTE,
          xt_PINGPONG_MESSAGE_TYPE_COUNT)) {
    xt_core_trace_exit("x_net_server_register_engine");
  }

  if (!xt_net_server_system_start(server)) {
    xt_core_trace_exit("x_net_server_start");
  }

  xt_net_server_system_destroy(server);
  xt_core_log_destroy(log);

  return 0;
}
