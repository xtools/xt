#include "xt/core/content.h"
#include "xt/core/tools.h"
#include "xt/net/engine/thread.h"
#include "xt/net/http/message.h"
#include "xt/net/server/system.h"
#include "xt/net/test/message.h"
#include "xt/net/test/http/engine.h"

struct xt_net_test_http_engine_t {
  void *things_the_engine_needs_to_share;
  xt_net_server_system_t *xt_net_server;
  void *custom_server_object;
};

static xt_net_message_status_t handle_about(void *engine_object,
    void *message_object);

static xt_net_message_status_t handle_not_found(void *engine_object,
    void *message_object);

xt_net_message_status_t handle_about(void *engine_object,
    void *message_object)
{
  assert(engine_object);
  assert(message_object);
  xt_net_test_http_engine_t *engine;
  xt_net_http_message_t *message;
  xt_net_http_message_t *response_message;
  int client_socket;
  xt_net_server_system_t *xt_net_server;
  char *body;
  unsigned long body_size;

  engine = engine_object;
  xt_net_server = engine->xt_net_server;
  message = message_object;
  client_socket = xt_core_message_get_client_socket(message);
  body = "about this site";
  body_size = strlen(body);

  response_message = xt_net_http_message_create(client_socket,
      XT_NET_HTTP_METHOD_UNKNOWN, XT_NET_HTTP_STATUS_OK,
      XT_NET_HTTP_MESSAGE_NO_RESOURCE_PATH, XT_NET_HTTP_VERSION_1_0,
      XT_NET_HTTP_MESSAGE_NO_HEADERS);
  if (response_message) {
    xt_net_http_message_set_body(response_message,
        XT_CORE_CONTENT_MIME_TEXT_HTML, body, body_size);
  } else {
    xt_core_trace("x_net_http_message_create");
  }

  if (!xt_net_server_system_send_message(xt_net_server, response_message)) {
    xt_core_trace("x_net_server_send_message");
  }

  return XT_NET_MESSAGE_STATUS_HANDLED;
}

xt_net_message_status_t handle_not_found(void *engine_object,
    void *message_object)
{
  assert(engine_object);
  assert(message_object);
  xt_net_test_http_engine_t *engine;
  xt_net_http_message_t *message;
  xt_net_http_message_t *response_message;
  int client_socket;
  xt_net_server_system_t *xt_net_server;
  char *body;
  unsigned long body_size;

  engine = engine_object;
  xt_net_server = engine->xt_net_server;
  message = message_object;
  client_socket = xt_core_message_get_client_socket(message);
  body = "test_http_engine cannot handle this request";
  body_size = strlen(body);

  response_message = xt_net_http_message_create(client_socket,
      XT_NET_HTTP_METHOD_UNKNOWN, XT_NET_HTTP_STATUS_NOT_FOUND,
      XT_NET_HTTP_MESSAGE_NO_RESOURCE_PATH, XT_NET_HTTP_VERSION_1_0,
      XT_NET_HTTP_MESSAGE_NO_HEADERS);
  if (response_message) {
    xt_net_http_message_set_body(response_message,
        XT_CORE_CONTENT_MIME_TEXT_HTML, body, body_size);
  } else {
    xt_core_trace("x_net_http_message_create");
  }

  if (!xt_net_server_system_send_message(xt_net_server, response_message)) {
    xt_core_trace("x_net_server_send_message");
  }

  return XT_NET_MESSAGE_STATUS_HANDLED;
}

void *xt_net_test_http_engine_create(xt_net_server_system_t *xt_net_server,
    void *custom_server_object)
{
  assert(xt_net_server);
  assert(custom_server_object);
  xt_net_test_http_engine_t *engine;

  printf("test_http_engine creating\n");

  engine = malloc(sizeof *engine);
  if (engine) {
    engine->xt_net_server = xt_net_server;
    engine->custom_server_object = custom_server_object;
  }

  return engine;
}

void xt_net_test_http_engine_destroy(void *engine_object)
{
  assert(engine_object);

  printf("test_http_engine destroying\n");
  free(engine_object);
}

xt_net_server_system_handle_message_f xt_net_test_http_engine_get_handler_for_message
(void *engine_object, void *_http_message_object)
{
  assert(engine_object);
  assert(_http_message_object);
  /*  xt_net_test_http_engine_t *engine;  */
  xt_net_server_system_handle_message_f handler;
  xt_net_http_message_t *_http_message;
  char *resource_name;

  /*  engine = engine_object;  */
  _http_message = _http_message_object;

  resource_name = xt_net_http_message_get_resource_name(_http_message);

  if (0 == strcmp("/", resource_name)) {
    handler = handle_about;

  } else if (0 == strcmp("/about", resource_name)) {
    handler = handle_about;

  } else {
    handler = handle_not_found;

  }

  return handler;
}

void xt_net_test_http_engine_maintain(void *engine_object)
{
  assert(engine_object);
  /*  xt_net_test_http_engine_t *engine;  */

  /*  engine = engine_object;  */
}

void xt_net_test_http_engine_run(void *engine_thread_object)
{
  assert(engine_thread_object);
  xt_net_engine_thread_t *engine_thread;
  xt_net_test_http_engine_t *engine;
  xt_net_server_system_t *xt_net_server;
  unsigned short thread_index;

  engine_thread = engine_thread_object;
  engine = engine_thread->engine_object;
  xt_net_server = engine->xt_net_server;
  thread_index = engine_thread->thread_index;

  xt_net_server_system_process_messages
    (xt_net_server, XT_NET_ENGINE_HTTP, thread_index);
}

void xt_net_test_http_engine_start(void *engine_thread_object)
{
  assert(engine_thread_object);
  /*  xt_net_engine_thread_t *engine_thread;  */
  /*  xt_net_test_http_engine_t *engine;  */

  /*  engine_thread = engine_thread_object;  */
  /*  engine = engine_thread->engine_object;  */

  printf("test_http_engine starting\n");
}

void xt_net_test_http_engine_stop(void *engine_thread_object)
{
  assert(engine_thread_object);
  /*  xt_net_engine_thread_t *engine_thread;  */
  /*  xt_net_test_http_engine_t *engine;  */

  /*  engine_thread = engine_thread_object;  */
  /*  engine = engine_thread->engine_object;  */

  printf("test_http_engine stopping\n");
}
