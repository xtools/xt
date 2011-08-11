#ifndef xt_net_http_post_h
#define xt_net_http_post_h

#include "xt/core/object.h"
#include "xt/net/post/stats.h"

struct xt_net_http_post_t;
typedef struct xt_net_http_post_t xt_net_http_post_t;

int xt_net_http_post_compare(void *http_post_object_a,
    void *http_post_object_b);

xt_core_bool_t xt_net_http_post_compare_equal(void *http_post_object_a,
    void *http_post_object_b);

void *xt_net_http_post_create(int socket);

void *xt_net_http_post_create_decoy(int socket);

void xt_net_http_post_destroy(void *http_post_object);

void xt_net_http_post_destroy_decoy(void *http_post_object);

time_t xt_net_http_post_get_last_receive_activity_time(void *http_post_object);

int xt_net_http_post_get_socket(void *http_post_object);

void xt_net_http_post_get_stats(void *http_post_object,
    xt_net_post_stats_t *post_stats);

xt_core_bool_t xt_net_http_post_is_socket_closed(void *http_post_object);

unsigned long xt_net_http_post_mod(void *post_object, unsigned long divisor);

void *xt_net_http_post_receive_message(void *http_post_object);

void xt_net_http_post_receive_messages(void *http_post_object);

xt_core_bool_t xt_net_http_post_send_message(void *http_post_object,
    void *http_message_object);

void xt_net_http_post_send_messages(void *http_post_object);

#endif
