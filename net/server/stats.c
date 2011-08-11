#include "xt/core/basic/long.h"
#include "xt/core/basic/short.h"
#include "xt/core/message.h"
#include "xt/core/tools.h"
#include "xt/net/server/stats.h"

xt_core_bool_t xt_net_server_system_add_stats_to_message
(xt_net_server_stats_t *server_stats, xt_core_message_t *message)
{
  assert(message);
  assert(server_stats);
  xt_core_bool_t success;

  success = xt_core_bool_true;

  if (!xt_core_basic_long_add_to_message(&server_stats->engine_count, message)) {
    success = xt_core_bool_false;
  }

  if (!xt_core_basic_long_add_to_message
      (&server_stats->server_send_message_failure_count, message)) {
    success = xt_core_bool_false;
  }

  if (!xt_core_basic_long_add_to_message
      (&server_stats->engine_cant_handle_message_now_count, message)) {
    success = xt_core_bool_false;
  }

  if (!xt_core_basic_long_add_to_message
      (&server_stats->engine_handled_message_count, message)) {
    success = xt_core_bool_false;
  }

  if (!xt_core_basic_long_add_to_message
      (&server_stats->engine_cant_handle_message_now_count, message)) {
    success = xt_core_bool_false;
  }

  if (!xt_core_basic_long_add_to_message
      (&server_stats->discarded_message_for_nonexistent_client_count,
          message)) {
    success = xt_core_bool_false;
  }

  if (!xt_core_basic_long_add_to_message
      (&server_stats->discarded_message_for_unregistered_engine_count,
          message)) {
    success = xt_core_bool_false;
  }

  if (!xt_core_basic_long_add_to_message
      (&server_stats->discarded_message_engine_inbox_add_failed_count,
          message)) {
    success = xt_core_bool_false;
  }

  if (!xt_core_basic_long_add_to_message(&server_stats->uptime_seconds, message)) {
    success = xt_core_bool_false;
  }

  if (!xt_core_basic_long_add_to_message(&server_stats->connected_client_count,
          message)) {
    success = xt_core_bool_false;
  }

  if (!xt_core_basic_short_add_to_message(&server_stats->max_threads, message)) {
    success = xt_core_bool_false;
  }

  if (!xt_core_basic_short_add_to_message(&server_stats->thread_count, message)) {
    success = xt_core_bool_false;
  }

  return success;
}

xt_net_server_stats_t *xt_net_server_system_create_stats_from_message
(xt_core_message_t *message)
{
  assert(message);
  xt_net_server_stats_t *server_stats;

  server_stats = malloc(sizeof *server_stats);
  if (server_stats) {
    server_stats->engine_count = xt_core_message_take_long_value(message);

    server_stats->server_send_message_failure_count
      = xt_core_message_take_long_value(message);

    server_stats->engine_cant_handle_message_now_count
      = xt_core_message_take_long_value(message);

    server_stats->engine_handled_message_count
      = xt_core_message_take_long_value(message);

    server_stats->engine_cant_handle_message_count
      = xt_core_message_take_long_value(message);

    server_stats->discarded_message_for_nonexistent_client_count
      = xt_core_message_take_long_value(message);

    server_stats->discarded_message_for_unregistered_engine_count
      = xt_core_message_take_long_value(message);

    server_stats->discarded_message_engine_inbox_add_failed_count
      = xt_core_message_take_long_value(message);

    server_stats->uptime_seconds
      = xt_core_message_take_long_value (message);

    server_stats->connected_client_count
      = xt_core_message_take_long_value(message);

    server_stats->max_threads = xt_core_message_take_short_value(message);

    server_stats->thread_count = xt_core_message_take_short_value(message);
  } else {
    xt_core_trace("malloc");
  }

  return server_stats;
}

void xt_net_server_stats_init(xt_net_server_stats_t *server_stats)
{
  server_stats->server_send_message_failure_count = 0;
  server_stats->engine_cant_handle_message_now_count = 0;
  server_stats->engine_handled_message_count = 0;
  server_stats->engine_cant_handle_message_count = 0;
  server_stats->discarded_message_for_nonexistent_client_count = 0;
  server_stats->discarded_message_for_unregistered_engine_count = 0;
  server_stats->discarded_message_engine_inbox_add_failed_count = 0;
}
