#ifndef xt_net_server_system_h
#define xt_net_server_system_h

#include "xt/case/list.h"
#include "xt/config/system.h"
#include "xt/core/engine.h"
#include "xt/core/imessage.h"
#include "xt/core/log.h"
#include "xt/core/message.h"
#include "xt/net/engine/iengine.h"
#include "xt/net/ip_address.h"
#include "xt/net/post/ipost.h"
#include "xt/net/server/stats.h"

#define XT_NET_SERVER_SYSTEM_MAX_ENGINES 64
#define XT_NET_SERVER_SYSTEM_NO_MAINTAIN_FUNCTION NULL
#define XT_NET_SERVER_SYSTEM_NO_CONFIG_SYSTEM NULL
#define XT_NET_SERVER_SYSTEM_DONT_CLOSE_UNRESPONSIVE_CLIENTS 0
#define XT_NET_MESSAGE_TYPE_COUNT_NONE 0

xt_net_server_system_t *xt_net_server_system_create(const char *name,
    unsigned short min_port, unsigned short max_port,
    unsigned short max_threads, xt_core_imessage_t *messagey,
    xt_net_post_ipost_t *ipost, xt_net_engine_get_name_f get_engine_name,
    xt_config_system_t *config_system, xt_core_log_t *log);

void xt_net_server_system_destroy(xt_net_server_system_t *server);

xt_core_bool_t xt_net_server_system_engine_run_thread_quiesce_requested
(xt_net_server_system_t *server, xt_net_engine_id_t engine_id,
    unsigned short thread_index);

xt_net_server_system_handle_message_f
xt_net_server_system_get_handler_for_message(xt_net_server_system_t *server,
    void *message_object);

xt_config_system_t *xt_net_server_system_get_config_system
(xt_net_server_system_t *server);

void xt_net_server_system_get_stats(xt_net_server_system_t *server,
    xt_net_server_stats_t *server_stats);

void xt_net_server_system_print_stats(xt_net_server_system_t *server);

void xt_net_server_system_process_messages(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id, unsigned short thread_index);

xt_core_bool_t xt_net_server_system_register_engine
(xt_net_server_system_t *server, xt_net_engine_id_t engine_id,
    void *custom_server_context, xt_net_engine_iengine_t *iengine,
    unsigned short min_run_threads, unsigned short max_run_threads,
    xt_net_maintain_t maintain_schedule, unsigned long message_type_count);

void xt_net_server_system_register_message_handler
(xt_net_server_system_t *server, xt_net_engine_id_t engine_id,
    unsigned long message_type,
    xt_net_server_system_handle_message_f message_handler);

xt_core_bool_t xt_net_server_system_send_message
(xt_net_server_system_t *server, void *message_object);

void xt_net_server_system_set_unresponsive_client_time_seconds
(xt_net_server_system_t *server,
    unsigned long unresponsive_client_time_seconds);

xt_core_bool_t xt_net_server_system_start(xt_net_server_system_t *server);

xt_case_list_t *xt_net_server_system_take_unprocessed_messages
(xt_net_server_system_t *server);

xt_case_list_t *xt_net_server_system_take_unsent_messages
(xt_net_server_system_t *server);

#endif
