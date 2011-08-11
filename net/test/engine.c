#include "xt/core/tools.h"
#include "xt/net/engine/thread.h"
#include "xt/net/pong.h"
#include "xt/net/server/system.h"
#include "xt/net/test/engine.h"
#include "xt/net/test/message.h"

struct xt_net_test_engine_t {
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
  xt_net_ping_t *ping;
  xt_core_message_t *pong_message;
  xt_net_test_engine_t *engine;
  xt_net_server_system_t *xt_net_server;
  xt_net_message_status_t message_status;
  int client_socket;
  xt_core_message_t *message;
  xt_net_pong_t *pong;
  char *ping_string;

  message = message_object;
  engine = engine_object;
  xt_net_server = engine->xt_net_server;
  client_socket = xt_core_message_get_client_socket(message);

  ping = xt_net_ping_create_from_message(message);
  if (ping) {
    ping_string = xt_net_ping_get_string(ping);
    if (ping_string) {
      pong = xt_net_pong_create(ping_string);
      if (pong) {
        pong_message = xt_net_test_message_create
          (client_socket, XT_NET_TEST_MESSAGE_PONG);
        if (pong_message) {
          if (xt_net_pong_add_to_message(pong, pong_message)) {
            if (xt_net_server_system_send_message(xt_net_server, pong_message)) {
              message_status = XT_NET_MESSAGE_STATUS_HANDLED;
            } else {
              message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
              xt_core_message_destroy(pong_message);
            }
          } else {
            message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
            xt_core_trace("x_net_pong_add_to_message");
            xt_core_message_destroy(pong_message);
          }
        } else {
          message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
          xt_core_trace("x_net_testmessage_create");
        }
        xt_net_pong_destroy(pong);
      } else {
        message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
        xt_core_trace("x_net_pong_create");
      }
    } else {
      message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
      xt_core_trace("x_net_ping_get_string");
    }
    xt_net_ping_destroy(ping);
  } else {
    message_status = XT_NET_MESSAGE_STATUS_CANT_HANDLE;
    xt_core_trace("x_net_ping_create_from_message");
  }

  return message_status;
}

void *xt_net_test_engine_create(xt_net_server_system_t *xt_net_server,
    void *custom_server_object)
{
  assert(xt_net_server);
  assert(custom_server_object);
  xt_net_test_engine_t *engine;

  printf("test engine creating context\n");

  engine = malloc(sizeof *engine);
  if (engine) {
    engine->xt_net_server = xt_net_server;
    engine->custom_server_object = custom_server_object;
  }

  xt_net_server_system_register_message_handler(xt_net_server,
      XT_NET_ENGINE_TEST, XT_NET_TEST_MESSAGE_PING, handle_ping);
  return engine;
}

void xt_net_test_engine_destroy(void *engine_object)
{
  assert(engine_object);

  printf("test engine destroying\n");
  free(engine_object);
}

xt_net_server_system_handle_message_f xt_net_test_engine_get_handler_for_message
(void *engine_object, void *message_object)
{
  assert(engine_object);
  assert(message_object);
  xt_net_test_engine_t *engine;
  xt_net_server_system_handle_message_f handler;
  xt_core_message_t *message;

  engine = engine_object;
  message = message_object;
  handler = xt_net_server_system_get_handler_for_message(engine->xt_net_server, message);

  return handler;
}

void xt_net_test_engine_maintain(void *engine_object)
{
  assert(engine_object);
  /*  xt_net_test_engine_t *engine;  */

  /*  engine = engine_object;  */
}

void xt_net_test_engine_run(void *engine_thread_object)
{
  assert(engine_thread_object);
  xt_net_engine_thread_t *engine_thread;
  xt_net_test_engine_t *engine;
  xt_net_server_system_t *server;
  unsigned short thread_index;

  engine_thread = engine_thread_object;
  engine = engine_thread->engine_object;
  server = engine->xt_net_server;
  thread_index = engine_thread->thread_index;

  xt_net_server_system_process_messages(server, XT_NET_ENGINE_TEST, thread_index);
}

void xt_net_test_engine_start(void *engine_thread_object)
{
  assert(engine_thread_object);
  /*  xt_net_engine_thread_t *engine_thread;  */
  /*  xt_net_test_engine_t *engine;  */

  /*  engine_thread = engine_thread_object;  */
  /*  engine = engine_thread->engine_object;  */

  printf("test engine starting\n");
}

void xt_net_test_engine_stop(void *engine_thread_object)
{
  assert(engine_thread_object);
  /*  xt_net_engine_thread_t *engine_thread;  */
  /*  xt_net_test_engine_t *engine;  */

  /*  engine_thread = engine_thread_object;  */
  /*  engine = engine_thread->engine_object;  */

  printf("test engine stopping\n");
}
