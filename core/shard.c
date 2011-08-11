#include "xt/core/shard.h"
#include "xt/core/standard.h"

void xt_core_shard_init(xt_core_shard_t *shard, unsigned long id,
    void *object)
{
  assert(shard);
  shard->id = id;
  shard->object = object;
}
