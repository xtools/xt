#include "xt/core/dayhour.h"
#include "xt/core/time.h"
#include "xt/core/tools.h"

int main(int argc, char *argv[])
{
  xt_core_time_t *time;
  xt_core_dayhour_t dayhour;
  unsigned long seconds;
  char *time_string;

  for (seconds = 1000000; seconds < 1000010; seconds++) {
    time = xt_core_time_create(seconds);
    if (time) {
      xt_core_time_get_dayhour(time, &dayhour);
      time_string = create_string_from_dayhour(&dayhour);
      if (time_string) {
        printf("%lu => %s\n", seconds, time_string);
        assert(seconds == dayhour.seconds
            + (dayhour.minutes * XT_CORE_SECONDS_PER_MINUTE)
            + (dayhour.hours * XT_CORE_SECONDS_PER_HOUR)
            + (dayhour.days * XT_CORE_SECONDS_PER_DAY));
        free(time_string);
      }
      xt_core_time_destroy(time);
    } else {
      xt_core_trace("x_core_time_create");
    }
  }

  return 0;
}
