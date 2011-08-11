#ifndef xt_net_engine_thread_h
#define xt_net_engine_thread_h

struct xt_net_engine_thread_t {
  void *engine_object;
  unsigned short thread_index;
};
typedef struct xt_net_engine_thread_t xt_net_engine_thread_t;

xt_net_engine_thread_t *xt_net_engine_create_thread(void *engine_object,
    unsigned short thread_index);

void xt_net_engine_destroy_thread(xt_net_engine_thread_t *engine_thread);

#endif
