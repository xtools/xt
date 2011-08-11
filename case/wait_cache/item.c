#include "xt/core/tools.h"
#include "xt/case/wait_cache/item.h"

struct xt_case_wait_cache_item_t {
  time_t birth_time;
  unsigned long lifespan_seconds;
  xt_core_bool_t expired;
  xt_core_uuid_t *object_uuid;
  void *object;
  xt_core_object_copy_f copy;
  xt_core_object_destroy_f destroy;
};

int xt_case_wait_cache_item_compare(void *item_object_a,
    void *item_object_b)
{
  assert(item_object_a);
  assert(item_object_b);
  xt_case_wait_cache_item_t *item_a;
  xt_case_wait_cache_item_t *item_b;

  item_a = item_object_a;
  item_b = item_object_b;

  return xt_core_uuid_compare(item_a->object_uuid, item_b->object_uuid);
}

void *xt_case_wait_cache_item_copy(void *item_object)
{
  xt_core_trace_exit("TODO: implement");
}

xt_case_wait_cache_item_t *xt_case_wait_cache_item_create
(xt_core_uuid_t *object_uuid, void *object, xt_core_object_copy_f copy,
    xt_core_object_destroy_f destroy, unsigned long lifespan_seconds)
{
  assert(object_uuid);
  assert(object);
  assert(copy);
  xt_case_wait_cache_item_t *item;

  item = malloc(sizeof *item);
  if (item) {
    item->birth_time = time(NULL);
    item->object = object;
    item->copy = copy;
    item->destroy = destroy;
    item->lifespan_seconds = lifespan_seconds;
    item->expired = xt_core_bool_false;
    item->object_uuid = xt_core_uuid_copy(object_uuid);
    if (!item->object_uuid) {
      xt_core_trace("x_core_uuid_copy");
      free(item);
      item = NULL;
    }
  } else {
    xt_core_trace("malloc");
  }

  return item;
}

xt_case_wait_cache_item_t *xt_case_wait_cache_item_create_decoy
(xt_core_uuid_t *object_uuid)
{
  assert(object_uuid);
  xt_case_wait_cache_item_t *item;

  item = malloc(sizeof *item);
  if (item) {
    item->object_uuid = xt_core_uuid_copy(object_uuid);
    if (!item->object_uuid) {
      xt_core_trace("x_core_uuid_copy");
      free(item);
      item = NULL;
    }
  } else {
    xt_core_trace("malloc");
  }

  return item;
}

void xt_case_wait_cache_item_destroy(void *item_object)
{
  assert(item_object);
  xt_case_wait_cache_item_t *item;

  item = item_object;

  xt_core_uuid_destroy(item->object_uuid);
  if (item->destroy) {
    item->destroy(item->object);
  }
  free(item);
}

void xt_case_wait_cache_item_destroy_decoy(void *item_object)
{
  assert(item_object);
  xt_case_wait_cache_item_t *item;

  item = item_object;

  xt_core_uuid_destroy(item->object_uuid);
  free(item);
}

void xt_case_wait_cache_item_expire(xt_case_wait_cache_item_t *item)
{
  item->expired = xt_core_bool_true;
}

void *xt_case_wait_cache_item_get_object(xt_case_wait_cache_item_t *item)
{
  return item->object;
}

void *xt_case_wait_cache_item_get_object_copy
(xt_case_wait_cache_item_t *item)
{
  return item->copy(item->object);
}

xt_core_uuid_t *xt_case_wait_cache_item_get_object_uuid
(xt_case_wait_cache_item_t *item)
{
  return item->object_uuid;
}

xt_core_bool_t xt_case_wait_cache_item_is_expired(void *item_object)
{
  assert(item_object);
  xt_case_wait_cache_item_t *item;
  xt_core_bool_t is_expired;
  time_t now;

  item = item_object;

  if (item->expired) {
    is_expired = xt_core_bool_true;
  } else {
    now = time(NULL);
    if ((now - item->birth_time) > item->lifespan_seconds) {
      is_expired = xt_core_bool_true;
    } else {
      is_expired = xt_core_bool_false;
    }
  }

  return is_expired;
}
