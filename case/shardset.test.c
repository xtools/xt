#include "xt/case/shardset.h"
#include "xt/core/string.h"
#include "xt/core/tools.h"

#define SHARD_COUNT XT_CASE_SHARDSET_MAX_SHARDS

int main(int argc, char *argv[])
{
  xt_case_shardset_t *shardset;
  char *string;

  shardset = xt_case_shardset_create(xt_core_string_compare,
      xt_core_string_compare_equal, xt_core_string_copy,
      XT_CORE_OBJECT_NO_DESTROY_F, xt_core_string_hash,
      xt_core_string_mod, SHARD_COUNT);
  if (!shardset) {
    xt_core_trace_exit("x_case_shardset_create");
  }

  xt_case_shardset_add(shardset, "apple");
  xt_case_shardset_add(shardset, "bicycle");
  xt_case_shardset_add(shardset, "color");

  xt_case_shardset_iterate_start(shardset);
  while ((string = xt_case_shardset_iterate_next(shardset))) {
    printf("%s\n", string);
    if (0 == strcmp("bicycle", string)) {
      xt_case_shardset_iterate_remove(shardset);
    }
  }

  printf("\n");

  xt_case_shardset_iterate_start(shardset);
  while ((string = xt_case_shardset_iterate_next(shardset))) {
    printf("%s\n", string);
  }

  return 0;
}
