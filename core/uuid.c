#include "xt/core/bool.h"
#include "xt/core/tools.h"
#include "xt/core/uuid.h"

#define NULL_UUID "a8fffbb4-675c-4896-81f0-6eccf8bd8760"

struct xt_core_uuid_t {
  uuid_t uuid;
  char *string;
};

xt_core_bool_t xt_core_uuid_add_to_message(void *uuid_object,
    xt_core_message_t *message)
{
  assert(uuid_object);
  assert(message);
  xt_core_bool_t success;
  char *uuid_string;
  xt_core_uuid_t *uuid;

  uuid = uuid_object;

  uuid_string = xt_core_uuid_get_string(uuid);
  if (xt_core_message_add_string(message, uuid_string)) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
    xt_core_trace("x_core_message_add_string");
  }

  return success;
}

int xt_core_uuid_compare(void *uuid_a_object, void *uuid_b_object)
{
  assert(uuid_a_object);
  assert(uuid_b_object);
  xt_core_uuid_t *uuid_a = uuid_a_object;
  xt_core_uuid_t *uuid_b = uuid_b_object;

  return uuid_compare(uuid_a->uuid, uuid_b->uuid);
}

xt_core_bool_t xt_core_uuid_compare_equal(void *uuid_a_object,
    void *uuid_b_object)
{
  assert(uuid_a_object);
  assert(uuid_b_object);
  return (0 == xt_core_uuid_compare(uuid_a_object, uuid_b_object));
}

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

void *xt_core_uuid_create_from_message(xt_core_message_t *message)
{
  assert(message);
  xt_core_uuid_t *uuid;
  char *uuid_string;

  uuid_string = xt_core_message_take_string(message);
  if (uuid_string) {
    uuid = xt_core_uuid_create_from_string(uuid_string);
    if (!uuid) {
      xt_core_trace("x_core_uuid_create_from_string");
    }
    free(uuid_string);
  } else {
    xt_core_trace("x_core_message_take_string");
    uuid = NULL;
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

xt_core_uuid_t *xt_core_uuid_create_null()
{
  return xt_core_uuid_create_from_string(NULL_UUID);
}

void xt_core_uuid_destroy(void *uuid_object)
{
  assert(uuid_object);
  xt_core_uuid_t *uuid;

  uuid = uuid_object;

  if (uuid->string) {
    if (strcmp(uuid->string, NULL_UUID) != 0) {
      free(uuid->string);
    }
  }
  free(uuid);
}

char *xt_core_uuid_get_as_string(void *uuid_object)
{
  assert(uuid_object);
  char *s;

  s = strdup(xt_core_uuid_get_string(uuid_object));
  if (!s) {
    xt_core_trace("strdup");
  }

  return s;
}

unsigned long xt_core_uuid_get_memory_size_bytes(xt_core_uuid_t *uuid)
{
  assert(uuid);
  unsigned long size;

  size = sizeof *uuid;
  size += sizeof(uuid_t);

  return size;
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

unsigned long xt_core_uuid_hash(xt_core_uuid_t *uuid)
{
  assert(uuid);
  unsigned long hash;

  xt_core_uuid_get_string(uuid);
  hash = xt_core_hash_djb2_xor(uuid->string);

  return hash;
}

void xt_core_uuid_init_iobject(xt_core_iobject_t *iobject)
{
  xt_core_iobject_init(iobject, xt_core_uuid_compare,
      xt_core_uuid_compare_equal, xt_core_uuid_copy,
      xt_core_uuid_destroy, xt_core_uuid_get_as_string,
      xt_core_uuid_mod);
}

xt_core_bool_t xt_core_uuid_is_null(xt_core_uuid_t *uuid)
{
  assert(uuid);
  xt_core_bool_t is_null;
  xt_core_uuid_t *null_uuid;

  is_null = xt_core_bool_false;

  null_uuid = xt_core_uuid_create_null();
  if (null_uuid) {
    if (0 == xt_core_uuid_compare(uuid, null_uuid)) {
      is_null = xt_core_bool_true;
    }
    xt_core_uuid_destroy(null_uuid);
  } else {
    xt_core_trace("x_core_uuid_create_null");
  }

  return is_null;
}

#if defined XT_PLATFORM_DARWIN
#include "xt/core/uuid.impl.darwin.c"
#endif

#if defined XT_PLATFORM_LINUX
#include "xt/core/uuid.impl.linux.c"
#endif
