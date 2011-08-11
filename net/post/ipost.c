#include "xt/net/post/ipost.h"

void xt_net_post_ipost_init(xt_net_post_ipost_t *ipost,
    xt_net_post_ipost_compare_f compare,
    xt_net_post_ipost_create_f create,
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
    xt_core_object_compare_equal_f compare_equal)
{
  assert(ipost);

  ipost->compare = compare;
  ipost->create = create;
  ipost->create_decoy = create_decoy;
  ipost->destroy = destroy;
  ipost->destroy_decoy = destroy_decoy;

  ipost->get_last_receive_activity_time = get_last_receive_activity_time;

  ipost->get_socket = get_socket;
  ipost->get_stats = get_stats;
  ipost->receive_message = receive_message;
  ipost->receive_messages = receive_messages;
  ipost->send_message = send_message;
  ipost->send_messages = send_messages;
  ipost->socket_closed = socket_closed;

  ipost->mod = mod;
  ipost->compare_equal = compare_equal;
}
