#include "xt/case/mbin.h"
#include "xt/core/basic/unsigned_long.h"
#include "xt/core/tools.h"
#include "xt/core/uuid.h"

#define ITERATIONS 10000
#define TEST_USING_LONGS xt_core_bool_false

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
  xt_case_mbin_t *mbin;
  long *l;
  unsigned long n;
  unsigned long items_added;
  xt_core_uuid_t *uuid;
  xt_core_iobject_t iobject;

  printf("creating mbin...\n");
  if (TEST_USING_LONGS) {
    xt_core_basic_unsigned_long_init_iobject(&iobject);
    mbin = xt_case_mbin_create(&iobject, XT_CASE_MBIN_SET_TYPE_SET);
  } else {
    xt_core_uuid_init_iobject(&iobject);
    mbin = xt_case_mbin_create(&iobject, XT_CASE_MBIN_SET_TYPE_SET);
  }
  assert(mbin);

  printf("adding to mbin...\n");
  items_added = 0;
  for (n = 0; n < ITERATIONS; n++) {
    if (TEST_USING_LONGS) {
      l = malloc(sizeof *l);
      *l = random() % ITERATIONS;
      if (xt_case_mbin_add(mbin, l)) {
        /*  printf("added %lu\n", *l);  */
        items_added++;
      } else {
        xt_core_basic_unsigned_long_destroy(l);
      }
    } else {
      uuid = xt_core_uuid_create();
      if (xt_core_bool_false) {
        print_uuid(uuid, "uuid");
      }
      if (xt_case_mbin_add(mbin, uuid)) {
        items_added++;
      } else {
        xt_core_uuid_destroy(uuid);
      }
    }
  }
  printf("items added: %lu\n", items_added);
  printf("mbin size: %lu\n", xt_case_mbin_get_size(mbin));

  printf("removing from mbin...\n");
  for (n = 0; n < ITERATIONS; n++) {
    if (TEST_USING_LONGS) {
      l = malloc(sizeof *l);
      *l = random() % ITERATIONS;
      xt_case_mbin_remove(mbin, l);
      /*  printf("removed %lu\n", *l);  */
      free(l);
    }
  }
  printf("mbin size: %lu\n", xt_case_mbin_get_size(mbin));

  if (xt_core_bool_false) {
    printf("doing iterate_remove on the rest of the mbin\n");
    xt_case_mbin_iterate_start(mbin);
    if (TEST_USING_LONGS) {
      while ((l = xt_case_mbin_iterate_next(mbin))) {
        xt_case_mbin_iterate_remove(mbin);
      }
    } else {
      while ((uuid = xt_case_mbin_iterate_next(mbin))) {
        xt_case_mbin_iterate_remove(mbin);
      }
    }
    printf("mbin size: %lu\n", xt_case_mbin_get_size(mbin));
    assert(0 == xt_case_mbin_get_size(mbin));
  }

  printf("clearing mbin...\n");
  xt_case_mbin_clear(mbin);
  printf("mbin size: %lu\n", xt_case_mbin_get_size(mbin));
  assert(0 == xt_case_mbin_get_size(mbin));

  printf("destroying mbin...\n");
  xt_case_mbin_destroy(mbin);

  return 0;
}
