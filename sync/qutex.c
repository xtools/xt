#include "xt/sync/mutex.h"
#include "xt/sync/qutex.h"
#include "xt/core/tools.h"

struct xt_sync_qutex_t {
  unsigned long shared_user_count;
  xt_core_bool_t exclusive_user;
  xt_sync_mutex_t *mutex;
};

xt_sync_qutex_t *xt_sync_qutex_create()
{
  xt_sync_qutex_t *qutex;

  qutex = malloc(sizeof *qutex);
  if (qutex) {
    qutex->shared_user_count = 0;
    qutex->exclusive_user = xt_core_bool_false;
    qutex->mutex = xt_sync_mutex_create();
    if (!qutex->mutex) {
      xt_core_trace("x_core_mutex_create");
      free(qutex);
      qutex = NULL;
    }
  } else {
    xt_core_trace("malloc() failed");
  }

  return qutex;
}

void xt_sync_qutex_destroy(xt_sync_qutex_t *qutex)
{
  assert(qutex);
  xt_sync_mutex_destroy(qutex->mutex);
  free(qutex);
}

void xt_sync_qutex_lock_exclusive(xt_sync_qutex_t *qutex)
{
  assert(qutex);
  while (!xt_sync_qutex_trylock_exclusive(qutex)) {}
}

void xt_sync_qutex_lock_shared(xt_sync_qutex_t *qutex)
{
  assert(qutex);
  while (!xt_sync_qutex_trylock_shared(qutex)) {}
}

xt_core_bool_t xt_sync_qutex_trylock_exclusive(xt_sync_qutex_t *qutex)
{
  assert(qutex);
  xt_core_bool_t locked;

  xt_sync_mutex_lock(qutex->mutex);
  {
    if (!qutex->exclusive_user && (0 == qutex->shared_user_count)) {
      qutex->exclusive_user = xt_core_bool_true;
      locked = xt_core_bool_true;
    } else {
      locked = xt_core_bool_false;
    }
  }
  xt_sync_mutex_unlock(qutex->mutex);

  return locked;
}

xt_core_bool_t xt_sync_qutex_trylock_shared(xt_sync_qutex_t *qutex)
{
  assert(qutex);
  xt_core_bool_t locked;

  xt_sync_mutex_lock(qutex->mutex);
  {
    if (!qutex->exclusive_user) {
      qutex->shared_user_count++;
      locked = xt_core_bool_true;
    } else {
      locked = xt_core_bool_false;
    }
  }
  xt_sync_mutex_unlock(qutex->mutex);

  return locked;
}

void xt_sync_qutex_unlock_exclusive(xt_sync_qutex_t *qutex)
{
  xt_sync_mutex_lock(qutex->mutex);
  qutex->exclusive_user = xt_core_bool_false;
  xt_sync_mutex_unlock(qutex->mutex);
}

void xt_sync_qutex_unlock_shared(xt_sync_qutex_t *qutex)
{
  xt_sync_mutex_lock(qutex->mutex);
  qutex->shared_user_count--;
  xt_sync_mutex_unlock(qutex->mutex);
}
