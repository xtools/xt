#ifndef xt_net_exchange_h
#define xt_net_exchange_h

#include "xt/net/post/ipost.h"

struct xt_net_exchange_t;
typedef struct xt_net_exchange_t xt_net_exchange_t;

xt_net_exchange_t *xt_net_exchange_create(xt_net_post_ipost_t *ipost);

void xt_net_exchange_destroy(xt_net_exchange_t *exchange);

unsigned long xt_net_exchange_get_post_count(xt_net_exchange_t *exchange);

xt_core_bool_t xt_net_exchange_register_post(xt_net_exchange_t *exchange,
    void *post_object);

void xt_net_exchange_send_and_receive_messages(xt_net_exchange_t *exchange);

xt_core_bool_t xt_net_exchange_unregister_post(xt_net_exchange_t *exchange,
    int socket);

#endif
