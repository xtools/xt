#include "xt/core/standard.h"
#include "xt/net/engine/thread.h"

xt_net_engine_thread_t *xt_net_engine_create_thread(void *engine_object,
    unsigned short thread_index)
{
  xt_net_engine_thread_t *engine_thread;

  engine_thread = malloc(sizeof *engine_thread);
  if (engine_thread) {
    engine_thread->engine_object = engine_object;
    engine_thread->thread_index = thread_index;
  }

  return engine_thread;
}

void xt_net_engine_destroy_thread(xt_net_engine_thread_t *engine_thread)
{
  assert(engine_thread);
  free(engine_thread);
}
