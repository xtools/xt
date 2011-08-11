#ifndef xt_net_star_client_system_h
#define xt_net_star_client_system_h

#include "xt/core/log.h"
#include "xt/core/engine.h"
#include "xt/net/client/system.h"
#include "xt/net/star/client/stats.h"

struct xt_net_star_client_system_t;
typedef struct xt_net_star_client_system_t xt_net_star_client_system_t;

xt_core_bool_t xt_net_star_client_system_connect(xt_net_star_client_system_t *starclient);

xt_net_star_client_system_t *xt_net_star_client_system_create(xt_case_list_t *star_arm_ips,
    unsigned short star_arm_port_min, unsigned short star_arm_port_max,
    char *node_server_exclude_ip, unsigned short node_server_exclude_min_port,
    unsigned short node_server_exclude_max_port, void *custom_client_context,
    xt_core_log_t *log);

void xt_net_star_client_system_destroy(xt_net_star_client_system_t *starclient);

xt_net_client_system_t *xt_net_star_client_system_get_client(xt_net_star_client_system_t *starclient,
    int socket);

void xt_net_star_client_system_get_stats(xt_net_star_client_system_t *starclient,
    xt_net_star_client_stats_t *starclient_stats);

void xt_net_star_client_system_process_messages(xt_net_star_client_system_t *starclient);

xt_core_bool_t xt_net_star_client_system_register_engine(xt_net_star_client_system_t *starclient,
    xt_net_engine_id_t engine_id, unsigned long message_type_count);

void xt_net_star_client_system_register_message_handler(xt_net_star_client_system_t *starclient,
    xt_net_engine_id_t engine_id, unsigned long message_type,
    xt_net_client_system_handle_message_f message_handler);

xt_core_bool_t xt_net_star_client_system_send_message_to_any_arm
(xt_net_star_client_system_t *starclient, xt_core_message_t *message);

xt_core_bool_t xt_net_star_client_system_send_message_to_all_arms
(xt_net_star_client_system_t *starclient, xt_core_message_t *message);

void xt_net_star_client_system_set_unsent_messages_queue_size
(xt_net_star_client_system_t *starclient, unsigned long queue_size);

xt_core_bool_t xt_net_star_client_system_star_available(xt_net_star_client_system_t *starclient);

#endif
