#include "xt/core/bitarray.h"
#include "xt/core/basic/long.h"
#include "xt/core/tools.h"

struct xt_core_bitarray_t {
  unsigned long array_size;
  xt_core_bit_t *array;
};

int xt_core_bitarray_compare(void *bitarray_object_a,
    void *bitarray_object_b)
{
  assert(bitarray_object_a);
  assert(bitarray_object_b);
  xt_core_bitarray_t *bitarray_a;
  xt_core_bitarray_t *bitarray_b;
  int compare_result;
  unsigned long each_bit;
  xt_core_bit_t bit_a;
  xt_core_bit_t bit_b;

  bitarray_a = bitarray_object_a;
  bitarray_b = bitarray_object_b;

  compare_result = xt_core_basic_long_compare(&bitarray_a->array_size,
      &bitarray_b->array_size);
  if (0 == compare_result) {
    for (each_bit = 0; each_bit < bitarray_a->array_size; each_bit++) {
      bit_a = *(bitarray_a->array + each_bit);
      bit_b = *(bitarray_b->array + each_bit);
      if (bit_a < bit_b) {
        compare_result = -1;
        break;
      } else if (bit_a > bit_b) {
        compare_result = 1;
        break;
      }
    }
  }

  return compare_result;
}

void *xt_core_bitarray_copy(void *bitarray_object)
{
  assert(bitarray_object);
  xt_core_bitarray_t *bitarray;
  xt_core_bitarray_t *bitarray_copy;
  unsigned long index;

  bitarray = bitarray_object;

  bitarray_copy = xt_core_bitarray_create(bitarray->array_size);
  if (bitarray_copy) {
    for (index = 0; index < bitarray->array_size; index++) {
      *(bitarray_copy->array + index) = *(bitarray->array + index);
    }
  } else {
    xt_core_trace("x_core_bitarray_create");
  }

  return bitarray_copy;
}

xt_core_bitarray_t *xt_core_bitarray_create(unsigned long size)
{
  assert(size > 0);
  xt_core_bitarray_t *bitarray;
  unsigned long array_memory_size;

  bitarray = malloc(sizeof *bitarray);
  if (bitarray) {
    array_memory_size = size * sizeof(xt_core_bit_t);
    bitarray->array = malloc(array_memory_size);
    if (bitarray->array) {
      memset(bitarray->array, '\0', array_memory_size);
      bitarray->array_size = size;
    } else {
      xt_core_trace("malloc");
      free(bitarray);
      bitarray = NULL;
    }
  } else {
    xt_core_trace("malloc");
  }

  return bitarray;
}

void xt_core_bitarray_destroy(void *bitarray_object)
{
  assert(bitarray_object);
  xt_core_bitarray_t *bitarray;

  bitarray = bitarray_object;

  free(bitarray->array);
  free(bitarray);

}

xt_core_bit_t xt_core_bitarray_get_bit_actual(xt_core_bitarray_t *bitarray,
    unsigned long index)
{
  return *(bitarray->array + index);
}

void xt_core_bitarray_set_all(xt_core_bitarray_t *bitarray)
{
  memset(bitarray->array, 1, bitarray->array_size * sizeof(xt_core_bit_t));
}

void xt_core_bitarray_set_bit_actual(xt_core_bitarray_t *bitarray,
    unsigned long index, xt_core_bit_t value)
{
  *(bitarray->array + index) = value;
}

unsigned long xt_core_bitarray_get_size(xt_core_bitarray_t *bitarray)
{
  return bitarray->array_size;
}

unsigned long xt_core_bitarray_get_memory_size_bytes
(xt_core_bitarray_t *bitarray)
{
  assert(bitarray);
  unsigned long size;

  size = sizeof *bitarray;
  size += bitarray->array_size * sizeof(xt_core_bit_t);

  return size;
}

void xt_core_bitarray_unset_all(xt_core_bitarray_t *bitarray)
{
  memset(bitarray->array, 0, bitarray->array_size * sizeof(xt_core_bit_t));
}
