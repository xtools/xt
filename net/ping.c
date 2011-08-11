#include "xt/core/string.h"
#include "xt/core/tools.h"
#include "xt/net/ping.h"

struct xt_net_ping_t {
  char *string;
};

xt_core_bool_t xt_net_ping_add_to_message(void *ping_object,
    xt_core_message_t *message)
{
  assert(ping_object);
  assert(message);
  xt_core_bool_t success;
  xt_net_ping_t *ping;

  ping = ping_object;

  success = xt_core_message_add_string(message, ping->string);

  return success;
}

xt_net_ping_t *xt_net_ping_create(const char *string)
{
  assert(string);
  xt_net_ping_t *ping;

  ping = malloc(sizeof *ping);
  if (ping) {
    ping->string = strdup(string);
    if (!ping->string) {
      xt_core_trace("strdup");
      free(ping);
      ping = NULL;
    }
  } else {
    xt_core_trace("malloc");
  }

  return ping;
}

xt_net_ping_t *xt_net_ping_create_from_message(xt_core_message_t *message)
{
  assert(message);
  xt_net_ping_t *ping;
  char *string;

  string = xt_core_message_take_string(message);
  if (string) {
    ping = xt_net_ping_create(string);
    if (!ping) {
      xt_core_trace("x_net_ping_create");
    }
    free(string);
  } else {
    xt_core_trace("x_core_message_take_string");
    ping = NULL;
  }

  return ping;
}

void xt_net_ping_destroy(xt_net_ping_t *ping)
{
  assert(ping);

  free(ping->string);
  free(ping);
}

char *xt_net_ping_get_string(xt_net_ping_t *ping)
{
  return ping->string;
}
