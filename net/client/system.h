#ifndef xt_net_client_system_h
#define xt_net_client_system_h

#include "xt/core/log.h"
#include "xt/case/list.h"
#include "xt/core/message.h"
#include "xt/core/imessage.h"
#include "xt/net/post/ipost.h"

struct xt_net_client_system_t;
typedef struct xt_net_client_system_t xt_net_client_system_t;

typedef void (*xt_net_client_system_handle_message_f)
(void *custom_client_context, xt_core_message_t *message);

void *xt_net_client_system_copy(void *client_object);

xt_net_client_system_t *xt_net_client_system_create
(const char *server_ip_address, unsigned short server_min_port,
    unsigned short server_max_port, xt_net_engine_get_name_f get_engine_name,
    void *custom_client_context, xt_core_log_t *log);

void xt_net_client_system_destroy(void *client_object);

char *xt_net_client_system_get_as_string(void *client_object);

char *xt_net_client_system_get_server_ip_address
(xt_net_client_system_t *client);

unsigned short xt_net_client_system_get_server_port
(xt_net_client_system_t *client);

int xt_net_client_system_get_socket(xt_net_client_system_t *client);

xt_core_bool_t xt_net_client_system_is_connected_to_server
(xt_net_client_system_t *client);

void xt_net_client_system_print(void *client_object);

void xt_net_client_system_process_messages(xt_net_client_system_t *client);

xt_core_bool_t xt_net_client_system_register_engine
(xt_net_client_system_t *client, xt_net_engine_id_t engine_id,
    unsigned long message_type_count);

void xt_net_client_system_register_message_handler
(xt_net_client_system_t *client, xt_net_engine_id_t engine_id,
    unsigned long message_type,
    xt_net_client_system_handle_message_f message_handler);

xt_core_bool_t xt_net_client_system_send_message
(xt_net_client_system_t *client, xt_core_message_t *message);

xt_case_list_t *xt_net_client_system_take_unsent_messages
(xt_net_client_system_t *client);

#endif
