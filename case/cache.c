#include "xt/case/cache.h"
#include "xt/case/set.h"
#include "xt/core/tools.h"

struct xt_case_cache_t {
  xt_case_set_t *objects;
  xt_sync_qutex_t *objects_qutex;
  xt_core_object_evaluate_condition_f remove_condition;
  xt_core_iobject_t objects_iobject;
};

xt_core_bool_t xt_case_cache_add(xt_case_cache_t *cache,
    void *object)
{
  assert(cache);
  assert(object);
  xt_core_bool_t success;

  xt_sync_qutex_lock_exclusive(cache->objects_qutex);
  success = xt_case_set_add(cache->objects, object);
  xt_sync_qutex_unlock_exclusive(cache->objects_qutex);

  if (!success) {
    xt_core_trace("x_case_set_add");
  }

  return success;
}

void xt_case_cache_clear(xt_case_cache_t *cache)
{
  xt_sync_qutex_lock_exclusive(cache->objects_qutex);
  xt_case_set_clear(cache->objects);
  xt_sync_qutex_unlock_exclusive(cache->objects_qutex);
}

xt_case_cache_t *xt_case_cache_create(xt_core_object_compare_f compare,
    xt_core_object_copy_f copy, xt_core_object_destroy_f destroy,
    xt_core_object_evaluate_condition_f remove_condition)
{
  xt_case_cache_t *cache;
  xt_core_bool_t so_far_so_good;

  cache = malloc(sizeof *cache);
  if (cache) {
    cache->remove_condition = remove_condition;
    cache->objects_qutex = NULL;
    xt_core_iobject_init(&cache->objects_iobject, compare,
        XT_CORE_OBJECT_NO_COMPARE_EQUAL_F, copy, destroy,
        XT_CORE_OBJECT_NO_GET_AS_STRING_F, XT_CORE_OBJECT_NO_MOD_F);
    cache->objects = xt_case_set_create(&cache->objects_iobject);
    if (cache->objects) {
      so_far_so_good = xt_core_bool_true;
    } else {
      so_far_so_good = xt_core_bool_false;
      xt_core_trace("x_case_set_create");
    }
  } else {
    so_far_so_good = xt_core_bool_false;
    xt_core_trace("malloc() failed");
  }

  if (so_far_so_good) {
    cache->objects_qutex = xt_sync_qutex_create();
    if (!cache->objects_qutex) {
      so_far_so_good = xt_core_bool_false;
      xt_core_trace("x_core_qutex_create");
    }
  }

  if (!so_far_so_good) {
    if (cache->objects) {
      xt_case_set_destroy(cache->objects);
    }
    if (cache->objects_qutex) {
      xt_sync_qutex_destroy(cache->objects_qutex);
    }
    free(cache);
    cache = NULL;
  }

  return cache;
}

void xt_case_cache_destroy(xt_case_cache_t *cache)
{
  assert(cache);
  xt_case_set_destroy(cache->objects);
  xt_sync_qutex_destroy(cache->objects_qutex);
}

void *xt_case_cache_find(xt_case_cache_t *cache,
    void *object)
{
  assert(cache);
  assert(object);
  void *found_object;

  xt_sync_qutex_lock_shared(cache->objects_qutex);
  found_object = xt_case_set_find(cache->objects, object);
  xt_sync_qutex_unlock_shared(cache->objects_qutex);

  return found_object;
}

void *xt_case_cache_find_copy(xt_case_cache_t *cache,
    void *object)
{
  assert(cache);
  assert(object);
  void *object_copy;

  xt_sync_qutex_lock_shared(cache->objects_qutex);
  object_copy = xt_case_set_find_copy(cache->objects, object);
  xt_sync_qutex_unlock_shared(cache->objects_qutex);

  return object_copy;
}

unsigned long xt_case_cache_get_size(xt_case_cache_t *cache)
{
  assert(cache);
  unsigned long size;

  xt_sync_qutex_lock_shared(cache->objects_qutex);
  size = xt_case_set_get_size(cache->objects);
  xt_sync_qutex_unlock_shared(cache->objects_qutex);

  return size;
}

void xt_case_cache_remove_objects(xt_case_cache_t *cache)
{
  assert(cache);
  void *object;

  xt_sync_qutex_lock_shared(cache->objects_qutex);
  {
    xt_case_set_iterate_start(cache->objects);
    while ((object = xt_case_set_iterate_next(cache->objects))) {
      if (cache->remove_condition(object)) {
        xt_case_set_iterate_remove(cache->objects);
      }
    }
  }
  xt_sync_qutex_unlock_shared(cache->objects_qutex);
}
