#include "xt/core/imessage.h"
#include "xt/core/message.h"
#include "xt/core/tools.h"
#include "xt/net/client/system.h"
#include "xt/net/post/system.h"
#include "xt/net/post/ipost.h"
#include "xt/net/server/system.h"
#include "xt/net/star/client/system.h"
#include "xt/net/star/node/system.h"

struct xt_net_star_node_system_t {
  xt_net_server_system_t *server;
  xt_core_imessage_t messagey;
  xt_net_post_ipost_t ipost;

  xt_net_star_client_system_t *starclient;

  pthread_t node_thread;

  xt_core_log_t *log;
};

static void xt_net_star_node_system_create_rollback(xt_net_star_node_system_t *node);

static void *node_thread(void *node_object);

xt_net_star_node_system_t *xt_net_star_node_system_create(char *node_ip, unsigned short node_min_port,
    unsigned short node_max_port, unsigned short node_max_threads,
    xt_case_list_t *peer_node_ips, unsigned short peer_node_port_min,
    unsigned short peer_node_port_max, xt_core_log_t *log)
{
  assert(peer_node_ips);
  xt_net_star_node_system_t *node;
  xt_core_bool_t so_far_so_good;

  node = malloc(sizeof *node);
  if (node) {
    xt_core_imessage_init(&node->messagey, xt_core_message_destroy,
        xt_core_message_get_client_socket,
        xt_core_message_get_engine_id, xt_core_message_get_type);
    xt_net_post_ipost_init(&node->ipost, xt_net_post_system_compare,
        xt_net_post_system_create, xt_net_post_system_create_decoy,
        xt_net_post_system_destroy, xt_net_post_system_destroy_decoy,
        xt_net_post_system_get_last_receive_activity_time,
        xt_net_post_system_get_socket, xt_net_post_system_get_stats,
        xt_net_post_system_receive_message,
        xt_net_post_system_receive_messages,
        xt_net_post_system_send_message, xt_net_post_system_send_messages,
        xt_net_post_system_is_socket_closed, xt_net_post_system_mod,
        xt_net_post_system_compare_equal);
    node->log = log;
    so_far_so_good = xt_core_bool_true;
  } else {
    xt_core_trace("malloc");
    so_far_so_good = xt_core_bool_false;
  }

  if (so_far_so_good) {
    node->server = xt_net_server_system_create("standard", node_min_port,
        node_max_port, node_max_threads, &node->messagey, &node->ipost,
        XT_NET_ENGINE_NO_GET_NAME_FUNCTION, XT_NET_SERVER_SYSTEM_NO_CONFIG_SYSTEM, log);
    if (node->server) {
      so_far_so_good = xt_core_bool_true;
    } else {
      xt_core_trace_exit("x_net_server_create failed()");
      so_far_so_good = xt_core_bool_false;
    }
  }

  if (so_far_so_good) {
    node->starclient = xt_net_star_client_system_create(peer_node_ips,
        peer_node_port_min, peer_node_port_max, node_ip, node_min_port,
        node_max_port, NULL, log);
    if (node->starclient) {
      so_far_so_good = xt_core_bool_true;
    } else {
      xt_core_trace_exit("x_net_starclient_create() failed()");
      so_far_so_good = xt_core_bool_false;
    }
  }

  if (!so_far_so_good && node) {
    xt_net_star_node_system_create_rollback(node);
    node = NULL;
  }

  return node;
}

void xt_net_star_node_system_create_rollback(xt_net_star_node_system_t *node)
{
  assert(node);

  if (node->server) {
    xt_core_request_stop();
    xt_net_server_system_destroy(node->server);
  }
  if (node->starclient) {
    xt_net_star_client_system_destroy(node->starclient);
  }
  free(node);
}

void xt_net_star_node_system_destroy(xt_net_star_node_system_t *node)
{
  assert(node);

  /*
    TODO: take unprocessed and unsent messages from the server and reroute
    them to another node
  */

  if (0 != pthread_join(node->node_thread, NULL)) {
    xt_core_trace("pthread_join");
  }
  xt_net_server_system_destroy(node->server);
  xt_net_star_client_system_destroy(node->starclient);
  free(node);
}

xt_net_server_system_t *xt_net_star_node_system_get_server(xt_net_star_node_system_t *node)
{
  return node->server;
}

void xt_net_star_node_system_get_server_stats(xt_net_star_node_system_t *node,
    xt_net_server_stats_t *server_stats)
{
  xt_net_server_system_get_stats(node->server, server_stats);
}

xt_net_star_client_system_t *xt_net_star_node_system_get_starclient(xt_net_star_node_system_t *node)
{
  return node->starclient;
}

xt_core_bool_t xt_net_star_node_system_register_engine(xt_net_star_node_system_t *node,
    xt_net_engine_id_t engine_id, void *custom_server_context,
    xt_net_engine_iengine_t *iengine, unsigned short min_run_threads,
    unsigned short max_run_threads, xt_net_maintain_t maintain_schedule,
    unsigned long message_type_count)
{
  return xt_net_server_system_register_engine(node->server, engine_id,
      custom_server_context, iengine, min_run_threads, max_run_threads,
      maintain_schedule, message_type_count);
}

void xt_net_star_node_system_register_message_handler(xt_net_star_node_system_t *node,
    xt_net_engine_id_t engine_id, unsigned long message_type,
    xt_net_server_system_handle_message_f message_handler)
{
  xt_net_server_system_register_message_handler(node->server, engine_id, message_type,
      message_handler);
}

xt_core_bool_t xt_net_star_node_system_send_message_to_all_peers(xt_net_star_node_system_t *node,
    xt_core_message_t *message)
{
  return xt_net_star_client_system_send_message_to_all_arms(node->starclient, message);
}

xt_core_bool_t xt_net_star_node_system_send_message_to_any_peer(xt_net_star_node_system_t *node,
    xt_core_message_t *message)
{
  return xt_net_star_client_system_send_message_to_any_arm(node->starclient, message);
}

xt_core_bool_t xt_net_star_node_system_send_message_to_client(xt_net_star_node_system_t *node,
    xt_core_message_t *message)
{
  return xt_net_server_system_send_message(node->server, message);
}

void xt_net_star_node_system_set_server_unresponsive_client_time_seconds(xt_net_star_node_system_t *node,
    unsigned long unresponsive_client_time_seconds)
{
  xt_net_server_system_set_unresponsive_client_time_seconds
    (node->server, unresponsive_client_time_seconds);
}

xt_core_bool_t xt_net_star_node_system_start(xt_net_star_node_system_t *node)
{
  assert(node);
  xt_core_bool_t success;

  if (xt_net_server_system_start(node->server)) {
    xt_net_star_client_system_connect(node->starclient);
    if (0 == pthread_create(&node->node_thread, NULL, node_thread, node)) {
      success = xt_core_bool_true;
    } else {
      xt_core_trace("pthread_create");
      success = xt_core_bool_false;
    }
  } else {
    success = xt_core_bool_false;
    xt_core_request_stop();
  }

  return success;
}

void *node_thread(void *node_object)
{
  assert(node_object);
  xt_net_star_node_system_t *node;

  node = node_object;

  while (!xt_core_stop_requested) {
    xt_net_star_client_system_process_messages(node->starclient);
    usleep(XT_CORE_STANDARD_SLEEP_MICROSECONDS);
  }

  return NULL;
}
