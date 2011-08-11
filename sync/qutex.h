#ifndef xt_sync_qutex_h
#define xt_sync_qutex_h

#include "xt/core/bool.h"
#include "xt/sync/mutex.h"

struct xt_sync_qutex_t;
typedef struct xt_sync_qutex_t xt_sync_qutex_t;

xt_sync_qutex_t *xt_sync_qutex_create();

void xt_sync_qutex_destroy(xt_sync_qutex_t *qutex);

void xt_sync_qutex_lock_exclusive(xt_sync_qutex_t *qutex);

void xt_sync_qutex_lock_shared(xt_sync_qutex_t *qutex);

xt_core_bool_t xt_sync_qutex_trylock_exclusive(xt_sync_qutex_t *qutex);

xt_core_bool_t xt_sync_qutex_trylock_shared(xt_sync_qutex_t *qutex);

void xt_sync_qutex_unlock_exclusive(xt_sync_qutex_t *qutex);

void xt_sync_qutex_unlock_shared(xt_sync_qutex_t *qutex);

#endif
