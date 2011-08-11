#include "xt/core/message.h"
#include "xt/core/imessage.h"
#include "xt/net/pingpong/engine.h"
#include "xt/net/pingpong/message.h"

xt_core_message_t *xt_pingpong_message_create(int client_socket,
    unsigned long message_type)
{
  xt_core_message_t *message;
  message = xt_core_message_create(client_socket, XT_NET_ENGINE_PING,
      message_type, XT_CORE_IMESSAGE_NULL_DATA, XT_CORE_IMESSAGE_ZERO_DATA_SIZE);
  return message;
}
