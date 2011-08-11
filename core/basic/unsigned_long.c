#include "xt/core/basic/unsigned_long.h"
#include "xt/core/tools.h"

int xt_core_basic_unsigned_long_compare(void *unsigned_long_a_object,
    void *unsigned_long_b_object)
{
  unsigned long *long_a;
  unsigned long *long_b;
  int compare;

  long_a = unsigned_long_a_object;
  long_b = unsigned_long_b_object;

  if (*long_a > *long_b) {
    compare = 1;
  } else if (*long_a < *long_b) {
    compare = -1;
  } else {
    compare = 0;
  }

  return compare;
}

xt_core_bool_t xt_core_basic_unsigned_long_compare_equal
(void *unsigned_long_a_object, void *unsigned_long_b_object)
{
  assert(unsigned_long_a_object);
  assert(unsigned_long_b_object);
  unsigned long *unsigned_long_a = unsigned_long_a_object;
  unsigned long *unsigned_long_b = unsigned_long_b_object;

  return *unsigned_long_a == *unsigned_long_b;
}

void *xt_core_basic_unsigned_long_copy(void *unsigned_long_object)
{
  assert(unsigned_long_object);
  unsigned long *long_value;
  unsigned long *long_copy;

  long_value = unsigned_long_object;
  long_copy = malloc(sizeof *long_copy);
  if (long_copy) {
    *long_copy = *long_value;
  } else {
    xt_core_trace("malloc");
  }

  return long_copy;
}

void xt_core_basic_unsigned_long_destroy(void *unsigned_long_object)
{
  free(unsigned_long_object);
}

char *xt_core_basic_unsigned_long_get_as_string(void *unsigned_long_object)
{
  assert(unsigned_long_object);
  unsigned long *unsigned_long;
  char *string;

  unsigned_long = unsigned_long_object;

  string = malloc(10 + 1);
  if (string) {
    snprintf(string, 10 + 1, "%lu", *unsigned_long);
  } else {
    xt_core_trace_exit("malloc");
  }

  return string;
}

void xt_core_basic_unsigned_long_init_iobject(xt_core_iobject_t *iobject)
{
  assert(iobject);
  xt_core_iobject_init(iobject, xt_core_basic_unsigned_long_compare,
      xt_core_basic_unsigned_long_compare_equal,
      xt_core_basic_unsigned_long_copy,
      xt_core_basic_unsigned_long_destroy,
      xt_core_basic_unsigned_long_get_as_string,
      xt_core_basic_unsigned_long_mod);
}

unsigned long xt_core_basic_unsigned_long_mod(void *unsigned_long_object,
    unsigned long divisor)
{
  assert(unsigned_long_object);
  unsigned long *unsigned_long = unsigned_long_object;
  return *unsigned_long % divisor;
}
