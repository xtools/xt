#include "xt/core/object.h"
#include "xt/core/tools.h"
#include "xt/net/engine/thread.h"
#include "xt/net/pingpong/engine.h"
#include "xt/net/pingpong/message.h"
#include "xt/net/pingpong/ping.h"
#include "xt/net/pingpong/pong.h"
#include "xt/net/server/system.h"

struct xt_pingpong_engine_t {
  void *things_the_engine_needs_to_share;
  xt_net_server_system_t *xt_net_server;
  void *custom_server_object;
};

static xt_net_message_status_t handle_ping(void *engine_object,
    void *message_object);

xt_net_message_status_t handle_ping(void *engine_object,
    void *message_object)
{
  assert(engine_object);
  assert(message_object);
  xt_pingpong_ping_t *ping;
  xt_pingpong_pong_t *pong;
  xt_core_message_t *pong_message;
  xt_pingpong_engine_t *engine;
  xt_net_server_system_t *xt_net_server;
  xt_net_message_status_t message_status;
  int client_socket;
  xt_core_message_t *message;
  char *ping_string;

  message = message_object;
  engine = engine_object;
  xt_net_server = engine->xt_net_server;
  client_socket = xt_core_message_get_client_socket(message);

  ping = xt_pingpong_ping_create_from_message(message);
  if (ping) {
    ping_string = xt_pingpong_ping_get_string(ping);
    if (ping_string) {
      pong = xt_pingpong_pong_create(ping_string);
      if (pong) {
        pong_message = xt_pingpong_message_create
          (client_socket, xt_PINGPONG_MESSAGE_PONG);
        if (pong_message) {
          if (xt_pingpong_pong_add_to_message(pong, pong_message)) {
            if (xt_net_server_system_send_message(xt_net_server, pong_message)) {
              message_status = XT_NET_MESSAGE_STATUS_HANDLED;
            } else {
              xt_core_trace("x_net_server_send_message() failed");
              message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
              xt_core_message_destroy(pong_message);
            }
          } else {
            xt_core_trace("x_pingpong_pong_add_to_message");
            message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
            xt_core_message_destroy(pong_message);
          }
        } else {
          xt_core_trace("x_pingpong_message_create");
          message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
        }
        xt_pingpong_pong_destroy(pong);
      } else {
        xt_core_trace("x_pingpong_pong_create");
        message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
      }
      xt_pingpong_ping_destroy(ping);
    } else {
      xt_core_trace("x_pingpong_ping_get_string");
      message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
    }
  } else {
    xt_core_trace("x_pingpong_ping_create");
    message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
  }

  return message_status;
}

void *xt_pingpong_engine_create(xt_net_server_system_t *xt_net_server,
    void *custom_server_object)
{
  assert(xt_net_server);
  assert(custom_server_object);
  xt_pingpong_engine_t *engine;

  engine = malloc(sizeof *engine);
  if (engine) {
    engine->xt_net_server = xt_net_server;
    engine->custom_server_object = custom_server_object;
  }

  xt_net_server_system_register_message_handler(xt_net_server, XT_NET_ENGINE_PING,
      xt_PINGPONG_MESSAGE_PING, handle_ping);

  return engine;
}

void xt_pingpong_engine_destroy(void *engine_object)
{
  free(engine_object);
}

xt_net_server_system_handle_message_f xt_pingpong_engine_get_handler_for_message
(void *engine_object, void *message_object)
{
  assert(engine_object);
  assert(message_object);
  xt_pingpong_engine_t *engine;
  xt_net_server_system_handle_message_f handler;
  xt_core_message_t *message;

  engine = engine_object;
  message = message_object;
  handler
    = xt_net_server_system_get_handler_for_message(engine->xt_net_server, message);

  return handler;
}

void xt_pingpong_engine_maintain(void *engine_object)
{
  assert(engine_object);
  xt_pingpong_engine_t *engine;
  xt_net_server_system_t *xt_net_server;

  engine = engine_object;
  xt_net_server = engine->xt_net_server;

  xt_net_server_system_print_stats(xt_net_server);
}

void xt_pingpong_engine_run(void *engine_thread_object)
{
  assert(engine_thread_object);
  xt_net_engine_thread_t *engine_thread;
  xt_pingpong_engine_t *engine;
  xt_net_server_system_t *xt_net_server;
  unsigned char thread_index;

  engine_thread = engine_thread_object;
  engine = engine_thread->engine_object;
  xt_net_server = engine->xt_net_server;
  thread_index = engine_thread->thread_index;

  xt_net_server_system_process_messages(xt_net_server, XT_NET_ENGINE_PING, thread_index);
}

void xt_pingpong_engine_start(void *engine_thread_object)
{
  assert(engine_thread_object);
  /*  xt_net_engine_thread_t *engine_thread;  */
  /*  xt_pingpong_engine_t *engine;  */

  /*  engine_thread = engine_thread_object;  */
  /*  engine = engine_thread->engine_object;  */

  printf("[ping] ping engine starting\n");
}

void xt_pingpong_engine_stop(void *engine_thread_object)
{
  assert(engine_thread_object);
  /*  xt_net_engine_thread_t *engine_thread;  */
  /*  xt_pingpong_engine_t *engine;  */

  /*  engine_thread = engine_thread_object;  */
  /*  engine = engine_thread->engine_object;  */

  printf("[ping] ping engine stopping\n");
}
