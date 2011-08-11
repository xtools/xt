#ifndef xt_net_star_client_stats_h
#define xt_net_star_client_stats_h

#include "xt/core/message.h"

struct xt_net_star_client_stats_t {
  unsigned long connected_server_count;
};
typedef struct xt_net_star_client_stats_t xt_net_star_client_stats_t;

xt_core_bool_t xt_net_star_client_system_add_stats_to_message
(xt_net_star_client_stats_t *starclient_stats, xt_core_message_t *message);

xt_net_star_client_stats_t *xt_net_star_client_system_create_stats_from_message
(xt_core_message_t *message);

#endif
