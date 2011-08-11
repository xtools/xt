#include "xt/core/object.h"
#include "xt/core/tools.h"

void xt_core_object_print(void *object,
    xt_core_object_get_as_string_f get_object_as_string)
{
  assert(object);
  assert(get_object_as_string);
  char *object_string;

  object_string = get_object_as_string(object);
  if (object_string) {
    printf("%s\n", object_string);
    free(object_string);
  } else {
    xt_core_trace("get_as_string");
  }
}
