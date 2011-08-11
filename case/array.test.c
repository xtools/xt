#include "xt/case/array.h"
#include "xt/core/standard.h"
#include "xt/core/string.h"

int main(int argc, char *argv[])
{
  xt_case_array_t *array;
  void *object;
  char *object_string;
  unsigned long index;
  unsigned long iteration;

  array = xt_case_array_create(4, xt_core_string_compare,
      xt_core_string_copy, xt_core_string_destroy);

  xt_case_array_add(array, 0, strdup("puppy"));
  xt_case_array_add(array, 1, strdup("rooster"));
  xt_case_array_add(array, 2, strdup("cow"));
  xt_case_array_add(array, 3, strdup("spots"));

  for (index = 0; index < 4; index++) {
    object = xt_case_array_find(array, index);
    if (object) {
      object_string = (char *) object;
      printf("%s\n", object_string);
    }
  }

  printf("\n");

  for (iteration = 0; iteration < 16; iteration++) {
    object = xt_case_array_find_random(array);
    if (object) {
      object_string = (char *) object;
      printf("%s\n", object_string);
    }
  }

  printf("\n");

  xt_case_array_iterate_start(array);
  while ((object = xt_case_array_iterate_next(array))) {
    object_string = (char *) object;
    printf("%s\n", object_string);
  }

  printf("\n");

  xt_case_array_iterate_start(array);
  while ((object = xt_case_array_iterate_next(array))) {
    object_string = (char *) object;
    printf("%s\n", object_string);
  }

  xt_case_array_destroy(array);

  xt_case_array_t *array2;
  array2 = xt_case_array_create(1, xt_core_string_compare,
      xt_core_string_copy, xt_core_string_destroy);
  xt_case_array_add(array2, 0, malloc(7));
  xt_case_array_remove(array2, 0);
  xt_case_array_destroy(array2);

  return 0;
}
