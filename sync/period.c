#include "xt/sync/period.h"
#include "xt/core/tools.h"

struct xt_sync_period_t {
  unsigned long seconds;
  time_t next_time_to_do_it;
};

xt_sync_period_t *xt_sync_period_create(unsigned long seconds)
{
  xt_sync_period_t *period;

  period = malloc(sizeof *period);
  if (period) {
    period->seconds = seconds;
    period->next_time_to_do_it = time(NULL);
  } else {
    xt_core_trace("malloc() failed");
  }

  return period;
}

void xt_sync_period_destroy(xt_sync_period_t *period)
{
  free(period);
}

xt_core_bool_t xt_sync_period_once(xt_sync_period_t *period)
{
  assert(period);
  xt_core_bool_t do_it_this_time;
  time_t current_time;

  if (period->seconds != 0) {
    current_time = time(NULL);

    if (current_time >= period->next_time_to_do_it) {
      period->next_time_to_do_it = current_time + period->seconds + 0;
      do_it_this_time = xt_core_bool_true;
    } else {
      do_it_this_time = xt_core_bool_false;
    }
  } else {
    do_it_this_time = xt_core_bool_true;
  }

  return do_it_this_time;
}
