#ifndef xt_net_star_node_system_h
#define xt_net_star_node_system_h

#include "xt/core/message.h"
#include "xt/net/server/system.h"
#include "xt/net/star/client/system.h"

struct xt_net_star_node_system_t;
typedef struct xt_net_star_node_system_t xt_net_star_node_system_t;

xt_net_star_node_system_t *xt_net_star_node_system_create(char *node_ip, unsigned short node_min_port,
    unsigned short node_max_port, unsigned short node_max_threads,
    xt_case_list_t *peer_node_ips, unsigned short peer_node_port_min,
    unsigned short peer_node_port_max, xt_core_log_t *log);

void xt_net_star_node_system_destroy(xt_net_star_node_system_t *node);

xt_net_server_system_t *xt_net_star_node_system_get_server(xt_net_star_node_system_t *node);

void xt_net_star_node_system_get_server_stats(xt_net_star_node_system_t *node,
    xt_net_server_stats_t *server_stats);

xt_net_star_client_system_t *xt_net_star_node_system_get_starclient(xt_net_star_node_system_t *node);

xt_core_bool_t xt_net_star_node_system_register_engine(xt_net_star_node_system_t *node,
    xt_net_engine_id_t engine_id, void *custom_server_context,
    xt_net_engine_iengine_t *iengine, unsigned short min_run_threads,
    unsigned short max_run_threads, xt_net_maintain_t maintain_schedule,
    unsigned long message_type_count);

void xt_net_star_node_system_register_message_handler(xt_net_star_node_system_t *node,
    xt_net_engine_id_t engine_id, unsigned long message_type,
    xt_net_server_system_handle_message_f message_handler);

xt_core_bool_t xt_net_star_node_system_send_message_to_all_peers(xt_net_star_node_system_t *node,
    xt_core_message_t *message);

xt_core_bool_t xt_net_star_node_system_send_message_to_any_peer(xt_net_star_node_system_t *node,
    xt_core_message_t *message);

xt_core_bool_t xt_net_star_node_system_send_message_to_client(xt_net_star_node_system_t *node,
    xt_core_message_t *message);

void xt_net_star_node_system_set_server_unresponsive_client_time_seconds
(xt_net_star_node_system_t *node, unsigned long unresponsive_client_time_seconds);

xt_core_bool_t xt_net_star_node_system_start(xt_net_star_node_system_t *node);

#endif
