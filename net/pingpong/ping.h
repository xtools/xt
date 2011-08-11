#ifndef xt_pingpong_ping_h
#define xt_pingpong_ping_h

#include "xt/core/bool.h"
#include "xt/core/message.h"

struct xt_pingpong_ping_t;
typedef struct xt_pingpong_ping_t xt_pingpong_ping_t;

xt_core_bool_t xt_pingpong_ping_add_to_message(void *ping_object,
    xt_core_message_t *message);

xt_pingpong_ping_t *xt_pingpong_ping_create(char *string);

xt_pingpong_ping_t *xt_pingpong_ping_create_from_message(xt_core_message_t *message);

void xt_pingpong_ping_destroy(xt_pingpong_ping_t *ping);

char *xt_pingpong_ping_get_string(xt_pingpong_ping_t *ping);

#endif
