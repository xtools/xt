#include "xt/case/map.h"
#include "xt/core/basic/long.h"
#include "xt/core/objects.h"
#include "xt/core/string.h"
#include "xt/core/tools.h"

int main(int argc, char *argv[])
{
  xt_case_map_t *map;

  xt_core_objects_init();

  char *a_left = "apple";
  unsigned long a_right = 0;

  char *b_left = "bear";
  unsigned long b_right = 1;

  unsigned long *xt_right;

  map = xt_case_map_create(&xt_core_objects.string_iobject,
      &xt_core_objects.long_iobject, XT_CASE_MAP_DONT_DESTROY);
  if (!map) {
    xt_core_trace_exit("x_case_map_create");
  }

  if (!xt_case_map_add(map, a_left, &a_right)) {
    xt_core_trace_exit("x_case_map_add");
  }

  if (!xt_case_map_add(map, b_left, &b_right)) {
    xt_core_trace_exit("x_case_map_add");
  }

  xt_case_map_print(map);

  xt_right = xt_case_map_find(map, b_left);
  if (xt_right) {
    printf(":%lu:\n", *xt_right);
  } else {
    xt_core_trace_exit("x_case_map_find");
  }

  xt_case_map_destroy(map);

  return 0;
}
