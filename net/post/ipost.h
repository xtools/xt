#ifndef xt_net_post_ipost_h
#define xt_net_post_ipost_h

#include "xt/core/object.h"
#include "xt/core/standard.h"
#include "xt/net/post/stats.h"

typedef int (*xt_net_post_ipost_compare_f)(void *a_object, void *b_object);

typedef void *(*xt_net_post_ipost_create_f)(int socket);

typedef void *(*xt_net_post_ipost_create_decoy_f)(int socket);

typedef void (*xt_net_post_ipost_destroy_f)(void *post_object);

typedef void (*xt_net_post_ipost_destroy_decoy_f)(void *post_object);

typedef time_t (*xt_net_post_ipost_get_last_receive_activity_time_f)
(void *post_object);

typedef int (*xt_net_post_ipost_get_socket_f)(void *post_object);

typedef void (*xt_net_post_ipost_get_stats_f)(void *post_object,
    xt_net_post_stats_t *post_stats);

typedef void *(*xt_net_post_ipost_receive_message_f)(void *post_object);

typedef void (*xt_net_post_ipost_receive_messages_f)(void *post_object);

typedef xt_core_bool_t (*xt_net_post_ipost_send_message_f)(void *post_object,
    void *message_object);

typedef void (*xt_net_post_ipost_send_messages_f)(void *post_object);

typedef xt_core_bool_t (*xt_net_post_ipost_socket_closed_f)(void *post_object);

struct xt_net_post_ipost_t {
  xt_net_post_ipost_compare_f compare;
  xt_net_post_ipost_create_f create;
  xt_net_post_ipost_create_decoy_f create_decoy;
  xt_net_post_ipost_destroy_f destroy;
  xt_net_post_ipost_destroy_decoy_f destroy_decoy;

  xt_net_post_ipost_get_last_receive_activity_time_f
  get_last_receive_activity_time;

  xt_net_post_ipost_get_socket_f get_socket;
  xt_net_post_ipost_get_stats_f get_stats;
  xt_net_post_ipost_receive_message_f receive_message;
  xt_net_post_ipost_receive_messages_f receive_messages;
  xt_net_post_ipost_send_message_f send_message;
  xt_net_post_ipost_send_messages_f send_messages;
  xt_net_post_ipost_socket_closed_f socket_closed;

  xt_core_object_mod_f mod;
  xt_core_object_compare_equal_f compare_equal;
};
typedef struct xt_net_post_ipost_t xt_net_post_ipost_t;

void xt_net_post_ipost_init(xt_net_post_ipost_t *ipost,
    xt_net_post_ipost_compare_f compare, xt_net_post_ipost_create_f create,
    xt_net_post_ipost_create_decoy_f create_decoy,
    xt_net_post_ipost_destroy_f destroy,
    xt_net_post_ipost_destroy_decoy_f destroy_decoy,

    xt_net_post_ipost_get_last_receive_activity_time_f
    get_last_receive_activity_time,

    xt_net_post_ipost_get_socket_f get_socket,
    xt_net_post_ipost_get_stats_f get_stats,
    xt_net_post_ipost_receive_message_f receive_message,
    xt_net_post_ipost_receive_messages_f receive_messages,
    xt_net_post_ipost_send_message_f send_message,
    xt_net_post_ipost_send_messages_f send_messages,
    xt_net_post_ipost_socket_closed_f socket_closed,

    xt_core_object_mod_f mod,
    xt_core_object_compare_equal_f compare_equal);

#endif
