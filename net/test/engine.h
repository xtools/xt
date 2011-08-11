#ifndef xt_net_test_engine_h
#define xt_net_test_engine_h

#include "xt/core/engine.h"
#include "xt/net/server/system.h"

struct xt_net_test_engine_t;
typedef struct xt_net_test_engine_t xt_net_test_engine_t;

void *xt_net_test_engine_create(xt_net_server_system_t *xt_net_server,
    void *custom_server_object);

void xt_net_test_engine_destroy(void *engine_object);

xt_net_server_system_handle_message_f
xt_net_test_engine_get_handler_for_message(void *engine_object,
    void *message_object);

void xt_net_test_engine_maintain(void *engine_object);

void xt_net_test_engine_run(void *engine_thread_object);

void xt_net_test_engine_start(void *engine_thread_object);

void xt_net_test_engine_stop(void *engine_thread_object);

#endif
