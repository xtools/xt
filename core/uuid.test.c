#include "xt/core/tools.h"
#include "xt/core/uuid.h"

#define REPETITIONS 32768

static void print_uuid(xt_core_uuid_t *uuid, const char *name);

static void print_uuid(xt_core_uuid_t *uuid, const char *name)
{
  assert(uuid);
  assert(name);
  char *uuid_string;

  uuid_string = xt_core_uuid_get_string(uuid);
  printf("%s=:%s:\n", name, uuid_string);
}

int main(int argc, char *argv[])
{
  xt_core_uuid_t *uuid_a;
  xt_core_uuid_t *uuid_a_copy;
  xt_core_uuid_t *uuid_b;
  int compare_result;
  xt_core_uuid_t *uuid_c;
  unsigned long each_uuid;

  uuid_a = xt_core_uuid_create();
  if (!uuid_a) {
    xt_core_trace_exit("x_core_uuid_create");
  }
  print_uuid(uuid_a, "uuid_a");

  uuid_b = xt_core_uuid_create();
  if (!uuid_b) {
    xt_core_trace_exit("x_core_uuid_create");
  }
  print_uuid(uuid_b, "uuid_b");

  uuid_a_copy = xt_core_uuid_copy(uuid_a);
  if (!uuid_a_copy) {
    xt_core_trace_exit("x_core_uuid_copy");
  }
  print_uuid(uuid_a_copy, "uuid_a_copy");

  compare_result = xt_core_uuid_compare(uuid_a, uuid_a);
  printf("cmp(uuid_a,uuid_a): %d\n", compare_result);

  compare_result = xt_core_uuid_compare(uuid_a, uuid_b);
  printf("cmp(uuid_a,uuid_b): %d\n", compare_result);

  compare_result = xt_core_uuid_compare(uuid_b, uuid_a);
  printf("cmp(uuid_b,uuid_a): %d\n", compare_result);

  compare_result = xt_core_uuid_compare(uuid_b, uuid_b);
  printf("cmp(uuid_b,uuid_b): %d\n", compare_result);

  compare_result = xt_core_uuid_compare(uuid_a, uuid_a_copy);
  printf("cmp(uuid_a,uuid_a_copy): %d\n", compare_result);

  if (xt_core_bool_false) {
    printf("creating/destroying a uuid %d times\n", REPETITIONS);
    for (each_uuid = 0; each_uuid < REPETITIONS; each_uuid++) {
      uuid_c = xt_core_uuid_create();
      if (!uuid_c) {
        xt_core_trace_exit("x_core_uuid_create");
      }
      xt_core_uuid_destroy(uuid_c);
    }
  }

  xt_core_uuid_destroy(uuid_a);
  xt_core_uuid_destroy(uuid_a_copy);
  xt_core_uuid_destroy(uuid_b);

  return 0;
}
