#ifndef xt_net_test_message_h
#define xt_net_test_message_h

#include "xt/net/ping.h"

#define XT_NET_TEST_MESSAGE_TYPE_COUNT 3
enum xt_net_test_message_t {
  XT_NET_TEST_MESSAGE_UNKNOWN = 0,
  XT_NET_TEST_MESSAGE_PING = 1,
  XT_NET_TEST_MESSAGE_PONG = 2
};
typedef enum xt_net_test_message_t xt_net_test_message_t;

xt_core_message_t *xt_net_test_message_create(int client_socket,
    unsigned long message_type);

#endif
