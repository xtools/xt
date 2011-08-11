#include "xt/core/tools.h"

int main(int argc, char *argv[])
{
  unsigned long n;
  unsigned long m;

  for (n = 0; n < 32; n++) {
    for (m = 0; m < 64; m++) {
      printf("%i", xt_core_toss_coin());
    }
    printf("\n");
  }

  return 0;
}
