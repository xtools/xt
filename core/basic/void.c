#include "xt/core/basic/void.h"
#include "xt/core/tools.h"

int xt_core_basic_void_compare(void *object_a, void *object_b)
{
  int compare;

  if (object_a > object_b) {
    compare = 1;
  } else if (object_a < object_b) {
    compare = -1;
  } else {
    compare = 0;
  }

  return compare;
}

void *xt_core_basic_void_copy(void *object)
{
  return object;
}

void xt_core_basic_void_destroy(void *object)
{
  free(object);
}

char *xt_core_basic_void_get_as_string(void *object)
{
  xt_core_trace_exit("TODO: implement/test");

  assert(object);
  char *string;

  string = malloc(10 + 1);
  if (string) {
    snprintf(string, 10 + 1, "%p", object);
  } else {
    xt_core_trace_exit("malloc");
  }

  return string;
}

void xt_core_basic_void_init_iobject(xt_core_iobject_t *iobject)
{
  assert(iobject);

  iobject->get_as_string = xt_core_basic_void_get_as_string;
  iobject->compare = xt_core_basic_void_compare;
  iobject->copy = xt_core_basic_void_copy;
  iobject->destroy = xt_core_basic_void_destroy;
}
