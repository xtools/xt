#ifndef xt_pingpong_message_h
#define xt_pingpong_message_h

#define xt_PINGPONG_MESSAGE_TYPE_COUNT 3
enum xt_pingpong_message_t {
  xt_PINGPONG_MESSAGE_UNKNOWN = 0,
  xt_PINGPONG_MESSAGE_PING = 1,
  xt_PINGPONG_MESSAGE_PONG = 2
};
typedef enum xt_pingpong_message_t xt_pingpong_message_t;

xt_core_message_t *xt_pingpong_message_create(int client_socket,
    unsigned long message_type);

#endif
