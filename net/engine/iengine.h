#ifndef xt_net_engine_iengine_h
#define xt_net_engine_iengine_h

#include "xt/core/object.h"
#include "xt/net/tools.h"

typedef void *(*xt_net_engine_iengine_create_f)
  (xt_net_server_system_t *server, void *custom_server_context_object);

typedef void (*xt_net_engine_iengine_destroy_f)(void *engine_object);

typedef xt_net_server_system_handle_message_f
(*xt_net_engine_iengine_get_handler_for_message_f)
(void *engine_object, void *message_object);

typedef void (*xt_net_engine_iengine_maintain_f)
(void *engine_thread_context_object);

typedef void (*xt_net_engine_iengine_run_f)
(void *engine_thread_context_object);

typedef void (*xt_net_engine_iengine_start_f)
(void *engine_thread_context_object);

typedef void (*xt_net_engine_iengine_stop_f)
(void *engine_thread_context_object);

struct xt_net_engine_iengine_t {
  xt_net_engine_iengine_create_f create;
  xt_net_engine_iengine_destroy_f destroy;
  xt_net_engine_iengine_get_handler_for_message_f get_handler_for_message;
  xt_net_engine_iengine_maintain_f maintain;
  xt_net_engine_iengine_run_f run;
  xt_net_engine_iengine_start_f start;
  xt_net_engine_iengine_stop_f stop;
};
typedef struct xt_net_engine_iengine_t xt_net_engine_iengine_t;

void xt_net_engine_iengine_init(xt_net_engine_iengine_t *iengine,
    xt_net_engine_iengine_create_f create,
    xt_net_engine_iengine_destroy_f destroy,
    xt_net_engine_iengine_get_handler_for_message_f get_handler_for_message,
    xt_net_engine_iengine_maintain_f maintain, xt_net_engine_iengine_run_f run,
    xt_net_engine_iengine_start_f start, xt_net_engine_iengine_stop_f stop);

#endif
