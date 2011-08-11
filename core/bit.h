#ifndef xt_core_bit_h
#define xt_core_bit_h

#include "xt/core/object.h"

enum xt_core_bit_t {
  xt_core_bit_zero = 0,
  xt_core_bit_one = 1,
};
typedef enum xt_core_bit_t xt_core_bit_t;

int xt_core_bit_compare(void *bit_a_object, void *bit_b_object);

void *xt_core_bit_copy(void *bit_object);

void xt_core_bit_destroy(void *bit_object);

#endif
