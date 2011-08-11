#ifndef xt_core_bool_h
#define xt_core_bool_h

#include "xt/core/declarations.h"

enum xt_core_bool_t {
  xt_core_bool_false = 0,
  xt_core_bool_true = 1,
};
typedef enum xt_core_bool_t xt_core_bool_t;

xt_core_bool_t xt_core_bool_add_to_message(void *bool_object,
    xt_core_message_t *message);

void *xt_core_bool_create_from_message(xt_core_message_t *message);

#endif
