#include "xt/core/string.h"
#include "xt/core/tools.h"
#include "xt/net/pong.h"

struct xt_net_pong_t {
  char *string;
};

xt_core_bool_t xt_net_pong_add_to_message(void *pong_object,
    xt_core_message_t *message)
{
  assert(pong_object);
  assert(message);
  xt_core_bool_t success;
  xt_net_pong_t *pong;

  pong = pong_object;

  if (xt_core_message_add_string(message, pong->string)) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
    xt_core_trace("x_core_message_add_string");
  }

  return success;
}

xt_net_pong_t *xt_net_pong_create(char *string)
{
  assert(string);
  xt_net_pong_t *pong;

  pong = malloc(sizeof *pong);
  if (pong) {
    pong->string = xt_core_string_copy(string);
    if (!pong->string) {
      xt_core_trace("x_core_string_copy");
      free(pong);
      pong = NULL;
    }
  } else {
    xt_core_trace("malloc");
  }

  return pong;
}

xt_net_pong_t *xt_net_pong_create_from_message(xt_core_message_t *message)
{
  assert(message);
  char *string;
  xt_net_pong_t *pong;

  string = xt_core_message_take_string(message);
  if (string) {
    pong = xt_net_pong_create(string);
    if (!pong) {
      xt_core_trace("x_net_pong_create");
    }
    free(string);
  } else {
    xt_core_trace("x_core_message_take_string");
    pong = NULL;
  }

  return pong;
}

void xt_net_pong_destroy(xt_net_pong_t *pong)
{
  assert(pong);

  free(pong->string);
  free(pong);
}
