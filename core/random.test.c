#include "xt/core/random.h"
#include "xt/core/tools.h"

#define XT_CORE_RANDOM_TEST_ITERATIONS 30000

#define XT_CORE_RANDOM_TEST_UNIFORM_PRECISION 0.01

#define XT_CORE_RANDOM_TEST_POISSON_PRECISION 0.05
#define XT_CORE_RANDOM_TEST_POISSON_MEAN 3

#define XT_CORE_RANDOM_TEST_GAUSSIAN_PRECISION 0.05
#define XT_CORE_RANDOM_TEST_GAUSSIAN_MEAN 5

int main(int argc, char *argv[])
{
  unsigned long ii = 0;
  double value = 0.0;
  double total = 0.0;
  double average = 0.0;

  srand(time(0));

  value = 0.0;
  total = 0.0;
  average = 0.0;

  for (ii = 0; ii < XT_CORE_RANDOM_TEST_ITERATIONS; ii++) {
    value = xt_core_random_01();
    total += value;
  }
  average = (total / XT_CORE_RANDOM_TEST_ITERATIONS);
  value = fabs(average - .5);

  if (value > XT_CORE_RANDOM_TEST_UNIFORM_PRECISION) {
    xt_core_trace_exit("FAILED: Uniform random average: %f", average);
  }

  value = 0.0;
  total = 0.0;
  average = 0.0;

  for (ii = 0; ii < XT_CORE_RANDOM_TEST_ITERATIONS; ii++) {
    value = (double) xt_core_random_poisson(XT_CORE_RANDOM_TEST_POISSON_MEAN);
    total += value;
  }
  average = (total / XT_CORE_RANDOM_TEST_ITERATIONS);
  value = fabs(average - XT_CORE_RANDOM_TEST_POISSON_MEAN);

  if (value > XT_CORE_RANDOM_TEST_POISSON_PRECISION) {
    xt_core_trace_exit("FAILED: Poisson random average: %f vs %f", average,
        (double) XT_CORE_RANDOM_TEST_POISSON_MEAN);
  }

  value = 0.0;
  total = 0.0;
  average = 0.0;

  for (ii = 0; ii < XT_CORE_RANDOM_TEST_ITERATIONS; ii++) {
    value = xt_core_random_gaussian(XT_CORE_RANDOM_TEST_GAUSSIAN_MEAN, 2.0);
    total += value;
  }
  average = (total / XT_CORE_RANDOM_TEST_ITERATIONS);
  value = fabs(average - XT_CORE_RANDOM_TEST_GAUSSIAN_MEAN);

  if (value > XT_CORE_RANDOM_TEST_GAUSSIAN_PRECISION) {
    xt_core_trace_exit("FAILED: Gaussian random average: %f vs %f",
        average, (double) XT_CORE_RANDOM_TEST_GAUSSIAN_MEAN);
  }

  return 0;
}
