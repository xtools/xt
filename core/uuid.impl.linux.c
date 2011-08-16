#include "xt/core/uuid.h"

int xt_core_uuid_compare(void *uuid_a_object, void *uuid_b_object)
{
  assert(uuid_a_object);
  assert(uuid_b_object);
  xt_core_uuid_t *uuid_a = uuid_a_object;
  xt_core_uuid_t *uuid_b = uuid_b_object;

  return uuid_compare(uuid_a->uuid, uuid_b->uuid);
}

/*
xt_core_bool_t xt_core_uuid_compare_equal(void *uuid_a_object, void *uuid_b_object)
{
  assert(uuid_a_object);
  assert(uuid_b_object);
  unsigned char *uuid_a = uuid_a_object;
  unsigned char *uuid_b = uuid_b_object;
  xt_core_bool_t equal = xt_core_bool_true;
  unsigned char i;

  for (i = 0; i < 16; i++) {
    if (*(uuid_a + i) != *(uuid_b + i)) {
      equal = xt_core_bool_false;
      break;
    }
  }

  return equal;
}
*/

void *xt_core_uuid_copy(void *uuid_object)
{
  assert(uuid_object);
  xt_core_uuid_t *original;
  xt_core_uuid_t *copy;

  original = uuid_object;

  copy = malloc(sizeof *copy);
  if (copy) {
    copy->string = NULL;
    uuid_copy(copy->uuid, original->uuid);
  } else {
    xt_core_trace("malloc");
  }

  return copy;
}

xt_core_uuid_t *xt_core_uuid_create()
{
  xt_core_uuid_t *uuid;

  uuid = malloc(sizeof *uuid);
  if (uuid) {
    uuid_generate(uuid->uuid);
    uuid->string = NULL;
  } else {
    xt_core_trace("malloc");
  }

  return uuid;
}

xt_core_uuid_t *xt_core_uuid_create_from_string(const char *uuid_string)
{
  xt_core_uuid_t *uuid;

  uuid = malloc(sizeof *uuid);
  if (uuid) {
    uuid->string = NULL;
    if (0 != uuid_parse(uuid_string, uuid->uuid)) {
      free(uuid);
      uuid = NULL;
    }
  } else {
    xt_core_trace("malloc");
  }

  return uuid;
}

char *xt_core_uuid_get_string(void *uuid_object)
{
  assert(uuid_object);
  xt_core_uuid_t *uuid;

  uuid = uuid_object;

  if (!uuid->string) {
    uuid->string = malloc(37);
    if (uuid->string) {
      uuid_unparse(uuid->uuid, uuid->string);
    } else {
      uuid->string = NULL_UUID;
      xt_core_trace("malloc");
    }
  }

  assert(uuid->string);
  return uuid->string;
}

unsigned long xt_core_uuid_mod(void *uuid_object, unsigned long divisor)
{
  assert(uuid_object);
  unsigned char *uuid = uuid_object;
  unsigned long dividend;
  unsigned long remainder;

  dividend = *((unsigned long *) (uuid + 12));
  remainder = dividend % divisor;
  /*  printf("%lu::%lu\n", dividend, remainder);  */

  return remainder;
}
