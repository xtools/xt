#ifndef xt_core_random_h
#define xt_core_random_h

double xt_core_random_01();

double xt_core_random_gaussian(double mean, double stddev);

unsigned long xt_core_random_poisson(double mean);

unsigned long xt_core_random_unsigned_long(unsigned long range);

#endif
