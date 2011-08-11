#include "xt/core/tools.h"

unsigned long xt_get_microseconds()
{
  struct timeval now;
  unsigned long microseconds;

  if (gettimeofday(&now, NULL) != -1) {
    microseconds = now.tv_usec;
  } else {
    xt_core_trace("gettimeofday");
    microseconds = 0;
  }

  return microseconds;
}
