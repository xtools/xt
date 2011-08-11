#ifndef xt_net_ping_h
#define xt_net_ping_h

#include "xt/core/message.h"

struct xt_net_ping_t;
typedef struct xt_net_ping_t xt_net_ping_t;

xt_core_bool_t xt_net_ping_add_to_message(void *ping_object,
    xt_core_message_t *message);

xt_net_ping_t *xt_net_ping_create(const char *string);

xt_net_ping_t *xt_net_ping_create_from_message(xt_core_message_t *message);

void xt_net_ping_destroy(xt_net_ping_t *ping);

char *xt_net_ping_get_string(xt_net_ping_t *ping);

#endif
