#include "xt/sync/mutex.h"
#include "xt/core/tools.h"

struct xt_sync_mutex_t {
  pthread_mutex_t mutex;
};

xt_sync_mutex_t *xt_sync_mutex_create()
{
  xt_sync_mutex_t *mutex;

  mutex = malloc(sizeof *mutex);
  if (mutex) {
    if (0 != pthread_mutex_init(&mutex->mutex, NULL)) {
      xt_core_trace("pthread_mutex_init");
      free(mutex);
      mutex = NULL;
    }
  } else {
    xt_core_trace("malloc() failed");
  }

  return mutex;
}

void xt_sync_mutex_destroy(xt_sync_mutex_t *mutex)
{
  assert(mutex);

  if (0 != pthread_mutex_destroy(&mutex->mutex)) {
    xt_core_trace("pthread_mutex_destroy");
  }
  free(mutex);
}

void xt_sync_mutex_lock(xt_sync_mutex_t *mutex)
{
  assert(mutex);

  if (0 != pthread_mutex_lock(&mutex->mutex)) {
    xt_core_trace("pthread_mutex_lock");
  }
}

xt_core_bool_t xt_sync_mutex_trylock(xt_sync_mutex_t *mutex)
{
  assert(mutex);
  xt_core_bool_t success;

  if (0 == pthread_mutex_lock(&mutex->mutex)) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

void xt_sync_mutex_unlock(xt_sync_mutex_t *mutex)
{
  if (0 != pthread_mutex_unlock(&mutex->mutex)) {
    xt_core_trace("pthread_mutex_unlock");
  }
}
