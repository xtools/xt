#include "xt/core/tools.h"
#include "xt/net/star/client/stats.h"

xt_core_bool_t xt_net_star_client_system_add_stats_to_message
(xt_net_star_client_stats_t *starclient_stats, xt_core_message_t *message)
{
  assert(message);
  assert(starclient_stats);
  xt_core_bool_t success;

  if (xt_core_message_add_long
      (message, &starclient_stats->connected_server_count)) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

xt_net_star_client_stats_t *xt_net_star_client_system_create_stats_from_message
(xt_core_message_t *message)
{
  assert(message);
  xt_net_star_client_stats_t *starclient_stats;

  starclient_stats = malloc(sizeof *starclient_stats);
  if (starclient_stats) {
    starclient_stats->connected_server_count
      = xt_core_message_take_long_value(message);
  } else {
    xt_core_trace("malloc");
  }

  return starclient_stats;
}
