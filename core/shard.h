#ifndef xt_core_shard_h
#define xt_core_shard_h

#include "xt/core/object.h"

struct xt_core_shard_t {
  unsigned long id;
  void *object;
};
typedef struct xt_core_shard_t xt_core_shard_t;

void xt_core_shard_init(xt_core_shard_t *shard, unsigned long id, void *object);

#endif
