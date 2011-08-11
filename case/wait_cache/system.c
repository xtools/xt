#include "xt/case/cache.h"
#include "xt/core/tools.h"
#include "xt/case/wait_cache/item.h"
#include "xt/case/wait_cache/system.h"

#define WAIT_ON_OBJECT_SLEEP_MICROSECONDS XT_CORE_STANDARD_SLEEP_MICROSECONDS

struct xt_case_wait_cache_system_t {
  xt_case_cache_t *container_cache;
  pthread_mutex_t mutex;
};

static void *find_copy_expire(xt_case_wait_cache_system_t *system,
    xt_core_uuid_t *object_uuid);

void *find_copy_expire(xt_case_wait_cache_system_t *system,
    xt_core_uuid_t *object_uuid)
{
  assert(system);
  assert(object_uuid);
  xt_case_wait_cache_item_t *item_decoy;
  xt_case_wait_cache_item_t *item;
  void *object_copy;

  object_copy = NULL;

  item_decoy = xt_case_wait_cache_item_create_decoy(object_uuid);
  if (item_decoy) {

    pthread_mutex_lock(&system->mutex);
    {
      item = xt_case_cache_find(system->container_cache, item_decoy);
      if (item) {
        object_copy = xt_case_wait_cache_item_get_object_copy(item);
        if (!object_copy) {
          xt_core_trace("x_wait_cache_item_get_object_copy");
        }
        xt_case_wait_cache_item_expire(item);
      }
    }
    pthread_mutex_unlock(&system->mutex);

    xt_case_wait_cache_item_destroy_decoy(item_decoy);
  } else {
    xt_core_trace("x_wait_cache_item_create_decoy");
  }

  return object_copy;
}

xt_core_bool_t xt_case_wait_cache_system_add
(xt_case_wait_cache_system_t *system, xt_core_uuid_t *object_uuid,
    void *object, xt_core_object_copy_f copy,
    xt_core_object_destroy_f destroy, unsigned long lifetime_seconds)
{
  assert(object_uuid);
  assert(object);
  assert(copy);
  xt_core_bool_t success;
  xt_case_wait_cache_item_t *item;

  success = xt_core_bool_false;

  item = xt_case_wait_cache_item_create(object_uuid, object, copy, destroy,
      lifetime_seconds);
  if (item) {
    success = xt_case_cache_add(system->container_cache, item);
    if (!success) {
      xt_case_wait_cache_item_destroy(item);
      xt_core_trace("x_case_cache_add");
    }
  } else {
    xt_core_trace("x_wait_cache_item_create");
  }

  return success;
}

void xt_case_wait_cache_system_clear(xt_case_wait_cache_system_t *system)
{
  xt_case_cache_clear(system->container_cache);
}

xt_case_wait_cache_system_t *xt_case_wait_cache_system_create()
{
  xt_case_wait_cache_system_t *system;
  xt_core_bool_t so_far_so_good;
  xt_core_bool_t mutex_needs_destroy;

  mutex_needs_destroy = xt_core_bool_false;

  system = malloc(sizeof *system);
  if (system) {
    system->container_cache = xt_case_cache_create
      (xt_case_wait_cache_item_compare, xt_case_wait_cache_item_copy,
          xt_case_wait_cache_item_destroy,
          xt_case_wait_cache_item_is_expired);
    if (system->container_cache) {
      so_far_so_good = xt_core_bool_true;
    } else {
      so_far_so_good = xt_core_bool_false;
      xt_core_trace("new");
    }
  } else {
    so_far_so_good = xt_core_bool_false;
    xt_core_trace("malloc");
  }

  if (so_far_so_good) {
    if (0 == pthread_mutex_init(&system->mutex, NULL)) {
      mutex_needs_destroy = xt_core_bool_true;
    } else {
      so_far_so_good = xt_core_bool_false;
      xt_core_trace("pthread_mutex_init");
    }
  }

  if (system && !so_far_so_good) {
    if (system->container_cache) {
      xt_case_cache_destroy(system->container_cache);
    }
    if (mutex_needs_destroy) {
      if (0 != pthread_mutex_destroy(&system->mutex)) {
        xt_core_trace("pthread_mutex_destroy");
      }
    }
    free(system);
    system = NULL;
  }

  return system;
}

void xt_case_wait_cache_system_destroy(xt_case_wait_cache_system_t *system)
{
  assert(system);
  xt_case_cache_destroy(system->container_cache);
  if (0 != pthread_mutex_destroy(&system->mutex)) {
    xt_core_trace("pthread_mutex_destroy");
  }
  free(system);
}

void *xt_case_wait_cache_system_find_wait_copy_expire
(xt_case_wait_cache_system_t *system, xt_core_uuid_t *object_uuid,
    unsigned long max_wait_seconds)
{
  assert(system);
  assert(object_uuid);
  void *object_copy;
  time_t start_wait_time;
  time_t current_time;
  unsigned long wait_time_seconds;

  start_wait_time = time(NULL);

  do {
    object_copy = find_copy_expire(system, object_uuid);
    if (!object_copy) {
      usleep(WAIT_ON_OBJECT_SLEEP_MICROSECONDS);
      current_time = time(NULL);
      wait_time_seconds = current_time - start_wait_time;
    }
  } while (!object_copy && (wait_time_seconds < max_wait_seconds));

  return object_copy;
}

unsigned long xt_case_wait_cache_system_get_size
(xt_case_wait_cache_system_t *system)
{
  return xt_case_cache_get_size(system->container_cache);
}

void xt_case_wait_cache_system_remove_objects
(xt_case_wait_cache_system_t *system)
{
  pthread_mutex_lock(&system->mutex);
  xt_case_cache_remove_objects(system->container_cache);
  pthread_mutex_unlock(&system->mutex);
}
