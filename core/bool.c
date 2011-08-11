#include "xt/core/bool.h"
#include "xt/core/message.h"

xt_core_bool_t xt_core_bool_add_to_message(void *bool_object,
    xt_core_message_t *message)
{
  return xt_core_message_add_bool(message, bool_object);
}

void *xt_core_bool_create_from_message(xt_core_message_t *message)
{
  return xt_core_message_take_bool(message);
}
