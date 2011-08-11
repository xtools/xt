#include "xt/core/engine.h"
#include "xt/core/message.h"
#include "xt/core/imessage.h"
#include "xt/core/tools.h"
#include "xt/net/test/engine.h"
#include "xt/net/test/message.h"

xt_core_message_t *xt_net_test_message_create(int client_socket,
    unsigned long message_type)
{
  return xt_core_message_create(client_socket, XT_NET_ENGINE_TEST,
      message_type, XT_CORE_IMESSAGE_NULL_DATA,
      XT_CORE_IMESSAGE_ZERO_DATA_SIZE);
}
