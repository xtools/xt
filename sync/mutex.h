#ifndef xt_sync_mutex_h
#define xt_sync_mutex_h

#include "xt/core/bool.h"

struct xt_sync_mutex_t;
typedef struct xt_sync_mutex_t xt_sync_mutex_t;

xt_sync_mutex_t *xt_sync_mutex_create();

void xt_sync_mutex_destroy(xt_sync_mutex_t *mutex);

void xt_sync_mutex_lock(xt_sync_mutex_t *mutex);

xt_core_bool_t xt_sync_mutex_trylock(xt_sync_mutex_t *mutex);

void xt_sync_mutex_unlock(xt_sync_mutex_t *mutex);

#endif
