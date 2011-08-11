#ifndef xt_sync_period_h
#define xt_sync_period_h

#include "xt/core/bool.h"

struct xt_sync_period_t;
typedef struct xt_sync_period_t xt_sync_period_t;

xt_sync_period_t *xt_sync_period_create(unsigned long seconds);

void xt_sync_period_destroy(xt_sync_period_t *period);

xt_core_bool_t xt_sync_period_once(xt_sync_period_t *period);

#endif
