#ifndef xt_net_post_system_h
#define xt_net_post_system_h

#include "xt/case/list.h"
#include "xt/core/message.h"
#include "xt/net/post/stats.h"

struct xt_net_post_system_t;
typedef struct xt_net_post_system_t xt_net_post_system_t;

int xt_net_post_system_compare(void *post_object_a, void *post_object_b);

xt_core_bool_t xt_net_post_system_compare_equal(void *post_object_a,
    void *post_object_b);

void *xt_net_post_system_create(int socket);

void *xt_net_post_system_create_decoy(int socket);

void xt_net_post_system_destroy(void *post_object);

void xt_net_post_system_destroy_decoy(void *post_object);

time_t xt_net_post_system_get_last_receive_activity_time(void *post_object);

int xt_net_post_system_get_socket(void *post_object);

void xt_net_post_system_get_stats(void *post_object,
    xt_net_post_stats_t *post_stats);

xt_core_bool_t xt_net_post_system_is_socket_closed(void *post_object);

unsigned long xt_net_post_system_mod(void *post_object, unsigned long divisor);

void *xt_net_post_system_receive_message(void *post_object);

void xt_net_post_system_receive_messages(void *post_object);

xt_core_bool_t xt_net_post_system_send_message(void *post_object,
    void *message_object);

void xt_net_post_system_send_messages(void *post_object);

xt_case_list_t *xt_net_post_system_take_unsent_messages
(xt_net_post_system_t *post);

#endif
