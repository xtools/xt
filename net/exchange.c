#include "xt/case/set.h"
#include "xt/core/tools.h"
#include "xt/net/exchange.h"

struct xt_net_exchange_t {
  xt_case_set_t *posts;
  xt_net_post_ipost_t *ipost;
  xt_core_iobject_t ipost_iobject;
};

xt_net_exchange_t *xt_net_exchange_create(xt_net_post_ipost_t *ipost)
{
  assert(ipost);
  xt_net_exchange_t *exchange;
  xt_core_bool_t success;

  exchange = malloc(sizeof *exchange);
  if (exchange) {
    success = xt_core_bool_true;
    exchange->ipost = ipost;
    xt_core_iobject_init(&exchange->ipost_iobject, ipost->compare,
        ipost->compare_equal, XT_CORE_OBJECT_NO_COPY_F,
        XT_CORE_OBJECT_NO_DESTROY_F, XT_CORE_OBJECT_NO_GET_AS_STRING_F,
        ipost->mod);
    exchange->posts = xt_case_set_create(&exchange->ipost_iobject);
    if (!exchange->posts) {
      xt_core_trace("x_case_set_create");
      success = xt_core_bool_false;
    }
  } else {
    xt_core_trace("malloc");
    success = xt_core_bool_false;
  }

  if (!success && exchange) {
    xt_case_set_destroy(exchange->posts);
    free(exchange);
    exchange = NULL;
  }

  return exchange;
}

void xt_net_exchange_destroy(xt_net_exchange_t *exchange)
{
  xt_case_set_destroy(exchange->posts);
  free(exchange);
}

unsigned long xt_net_exchange_get_post_count(xt_net_exchange_t *exchange)
{
  return xt_case_set_get_size(exchange->posts);
}

xt_core_bool_t xt_net_exchange_register_post(xt_net_exchange_t *exchange,
    void *post_object)
{
  return xt_case_set_add(exchange->posts, post_object);
}

void xt_net_exchange_send_and_receive_messages(xt_net_exchange_t *exchange)
{
  fd_set read_sockets;
  fd_set write_sockets;
  int max_socket;
  void *post_object;
  int socket;
  struct timeval select_timeout;

  FD_ZERO(&read_sockets);
  FD_ZERO(&write_sockets);
  max_socket = 0;
  select_timeout.tv_sec = 0;
  select_timeout.tv_usec = 0;

  xt_case_set_iterate_start(exchange->posts);
  while ((post_object = xt_case_set_iterate_next(exchange->posts))) {
    socket = exchange->ipost->get_socket(post_object);
    FD_SET(socket, &read_sockets);
    FD_SET(socket, &write_sockets);
    if (socket > max_socket) {
      max_socket = socket;
    }
  }

  if (select(max_socket + 1, &read_sockets, &write_sockets, NULL,
          &select_timeout) > 0) {
    xt_case_set_iterate_start(exchange->posts);
    while ((post_object = xt_case_set_iterate_next(exchange->posts))) {
      socket = exchange->ipost->get_socket(post_object);
      if (FD_ISSET(socket, &read_sockets)) {
        exchange->ipost->receive_messages(post_object);
      }
      if (FD_ISSET(socket, &write_sockets)) {
        exchange->ipost->send_messages(post_object);
      }
    }
  }
}

xt_core_bool_t xt_net_exchange_unregister_post(xt_net_exchange_t *exchange,
    int socket)
{
  void *decoy_post_object;
  xt_core_bool_t success;

  decoy_post_object = exchange->ipost->create_decoy(socket);
  if (decoy_post_object) {
    success = xt_core_bool_true;
    if (!xt_case_set_remove(exchange->posts, decoy_post_object)) {
      xt_core_do_nothing();
      xt_core_trace("x_case_set_remove");
      success = xt_core_bool_false;
    }
    exchange->ipost->destroy_decoy(decoy_post_object);
  } else {
    xt_core_trace("ipost->create_decoy");
    success = xt_core_bool_false;
  }

  return success;
}
