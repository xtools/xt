#include "xt/core/tools.h"

#define REPETITIONS 32768

int main(int argc, char *argv[])
{
  unsigned long each_toss;
  unsigned short toss_result;

  if (xt_core_bool_false) {
    printf("tossing a coin %d times\n", REPETITIONS);
    for (each_toss = 0; each_toss < REPETITIONS; each_toss++) {
      toss_result = xt_core_toss_coin();
    }
  }

  return 0;
}
