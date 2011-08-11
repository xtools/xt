#ifndef xt_net_types_h
#define xt_net_types_h

#include "xt/core/object.h"

enum xt_net_maintain_t {
  XT_NET_MAINTAIN_CONSTANTLY = 0,
  XT_NET_MAINTAIN_2_SECONDS = 2,
  XT_NET_MAINTAIN_4_SECONDS = 4,
  XT_NET_MAINTAIN_1_MINUTE = 60,
  XT_NET_MAINTAIN_1_HOUR = 60 * 60,
};
typedef enum xt_net_maintain_t xt_net_maintain_t;

enum xt_net_message_status_t {
  XT_NET_MESSAGE_STATUS_HANDLED,
  XT_NET_MESSAGE_STATUS_CANT_HANDLE,
  XT_NET_MESSAGE_STATUS_CANT_HANDLE_NOW,
};
typedef enum xt_net_message_status_t xt_net_message_status_t;

typedef xt_net_message_status_t (*xt_net_server_system_handle_message_f)
(void *engine_object, void *message_object);

struct xt_net_server_system_t;
typedef struct xt_net_server_system_t xt_net_server_system_t;

#endif
