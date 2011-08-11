#ifndef xt_sync_run_h
#define xt_sync_run_h

#include "xt/core/bool.h"

enum xt_sync_run_style_t {
  xt_SYNC_RUN_STYLE_UNKNOWN,
  xt_SYNC_RUN_STYLE_ITERATIONS,
  xt_SYNC_RUN_STYLE_WALL_TIME,
  xt_SYNC_RUN_STYLE_GOAL_REACHED,
  xt_SYNC_RUN_STYLE_INFINITE
};
typedef enum xt_sync_run_style_t xt_sync_run_style_t;

struct xt_sync_run_t;
typedef struct xt_sync_run_t xt_sync_run_t;

void xt_sync_run_consider_goal_reached(xt_sync_run_t *run);

xt_sync_run_t *xt_sync_run_create(xt_sync_run_style_t run_style);

void xt_sync_run_destroy(xt_sync_run_t *run);

void xt_sync_run_increment_iterations(xt_sync_run_t *run);

xt_core_bool_t xt_sync_run_is_complete(xt_sync_run_t *run);

void xt_sync_run_reset(xt_sync_run_t *run);

void xt_sync_run_set_max_iterations(xt_sync_run_t *run,
    unsigned long max_iterations);

void xt_sync_run_set_wall_time_expiration_seconds(xt_sync_run_t *run,
    unsigned long seconds);

#endif
