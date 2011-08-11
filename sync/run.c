#include "xt/sync/run.h"
#include "xt/core/tools.h"

struct xt_sync_run_t {
  xt_sync_run_style_t run_style;

  xt_core_bool_t goal_reached;

  unsigned long iterations;
  unsigned long max_iterations;

  time_t start_time;
  unsigned long wall_time_expiration_seconds;
};

static void reset(xt_sync_run_t *run);

void xt_sync_run_consider_goal_reached(xt_sync_run_t *run)
{
  run->goal_reached = xt_core_bool_true;
}

xt_sync_run_t *xt_sync_run_create(xt_sync_run_style_t run_style)
{
  xt_sync_run_t *run;

  run = malloc(sizeof *run);
  if (run) {
    run->run_style = run_style;
    reset(run);
  } else {
    xt_core_trace("malloc");
  }

  return run;
}

void xt_sync_run_destroy(xt_sync_run_t *run)
{
  assert(run);
  free(run);
}

void xt_sync_run_increment_iterations(xt_sync_run_t *run)
{
  run->iterations++;
}

xt_core_bool_t xt_sync_run_is_complete(xt_sync_run_t *run)
{
  assert(run);
  xt_core_bool_t is_complete;
  time_t current_time;

  switch (run->run_style) {

    case xt_SYNC_RUN_STYLE_ITERATIONS:
      if (run->iterations >= run->max_iterations) {
        is_complete = xt_core_bool_true;
      } else {
        is_complete = xt_core_bool_false;
      }
      break;

    case xt_SYNC_RUN_STYLE_WALL_TIME:
      time(&current_time);
      if ((current_time - run->start_time)
          >= run->wall_time_expiration_seconds) {
        is_complete = xt_core_bool_true;
      } else {
        is_complete = xt_core_bool_false;
      }
      break;

    case xt_SYNC_RUN_STYLE_GOAL_REACHED:
      is_complete = run->goal_reached;
      break;

    case xt_SYNC_RUN_STYLE_INFINITE:
      is_complete = xt_core_bool_false;
      break;

    case xt_SYNC_RUN_STYLE_UNKNOWN:
    default:
      is_complete = xt_core_bool_false;
      break;
  }

  return is_complete;
}

void xt_sync_run_reset(xt_sync_run_t *run)
{
  reset(run);
}

void xt_sync_run_set_max_iterations(xt_sync_run_t *run,
    unsigned long max_iterations)
{
  run->max_iterations = max_iterations;
}

void xt_sync_run_set_wall_time_expiration_seconds(xt_sync_run_t *run,
    unsigned long seconds)
{
  run->wall_time_expiration_seconds = seconds;
}

void reset(xt_sync_run_t *run)
{
  assert(run);

  run->goal_reached = xt_core_bool_false;
  run->iterations = 0;
  time(&run->start_time);
}
