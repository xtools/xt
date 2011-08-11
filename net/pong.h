#ifndef xt_net_pong_h
#define xt_net_pong_h

#include "xt/core/message.h"

struct xt_net_pong_t;
typedef struct xt_net_pong_t xt_net_pong_t;

xt_core_bool_t xt_net_pong_add_to_message(void *pong_object,
    xt_core_message_t *message);

xt_net_pong_t *xt_net_pong_create(char *string);

xt_net_pong_t *xt_net_pong_create_from_message(xt_core_message_t *message);

void xt_net_pong_destroy(xt_net_pong_t *pong);

#endif
