#ifndef xt_core_time_h
#define xt_core_time_h

#include "xt/core/dayhour.h"

struct xt_core_time_t;
typedef struct xt_core_time_t xt_core_time_t;

xt_core_time_t *xt_core_time_create(unsigned long seconds);

void xt_core_time_destroy(xt_core_time_t *time);

void xt_core_time_get_dayhour(xt_core_time_t *time,
    xt_core_dayhour_t *dayhour);

#endif
