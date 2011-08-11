#include "xt/core/time.h"
#include "xt/core/tools.h"

struct xt_core_time_t {
  unsigned long seconds;
};

xt_core_time_t *xt_core_time_create(unsigned long seconds)
{
  xt_core_time_t *time;

  time = malloc(sizeof *time);
  if (time) {
    time->seconds = seconds;
  } else {
    xt_core_trace("malloc");
  }

  return time;
}

void xt_core_time_destroy(xt_core_time_t *time)
{
  assert(time);
  free(time);
}

void xt_core_time_get_dayhour(xt_core_time_t *time,
    xt_core_dayhour_t *dayhour)
{
  assert(time);
  assert(dayhour);
  unsigned long seconds_left;

  seconds_left = time->seconds;

  dayhour->days = seconds_left / XT_CORE_SECONDS_PER_DAY;
  seconds_left -= (dayhour->days * XT_CORE_SECONDS_PER_DAY);

  dayhour->hours = seconds_left / XT_CORE_SECONDS_PER_HOUR;
  seconds_left -= (dayhour->hours * XT_CORE_SECONDS_PER_HOUR);

  dayhour->minutes = seconds_left / XT_CORE_SECONDS_PER_MINUTE;
  seconds_left -= (dayhour->minutes * XT_CORE_SECONDS_PER_MINUTE);

  dayhour->seconds = seconds_left;

}
