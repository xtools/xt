#include "xt/core/bitarray.h"
#include "xt/core/tools.h"

struct xt_core_bitarray_t {
  unsigned long array_size;

  unsigned char *array;
  unsigned long bit_count;
};

struct xt_core_bitarray_array_position_t {
  unsigned long char_index;
  unsigned short char_bit_index;
};
typedef struct xt_core_bitarray_array_position_t
xt_core_bitarray_array_position_t;

static void calculate_array_postion(unsigned long bit_index,
    xt_core_bitarray_array_position_t *array_position);

void calculate_array_postion(unsigned long bit_index,
    xt_core_bitarray_array_position_t *array_position)
{
  assert(array_position);
  array_position->char_index = bit_index / 8;
  array_position->char_bit_index = bit_index % 8;
}

int xt_core_bitarray_compare(void *bitarray_object_a,
    void *bitarray_object_b)
{
  xt_core_trace_exit("TODO: implement");
}

void *xt_core_bitarray_copy(void *bitarray_object)
{
  assert(bitarray_object);
  xt_core_bitarray_t *bitarray;
  xt_core_bitarray_t *bitarray_copy;

  bitarray = bitarray_object;

  bitarray_copy = xt_core_bitarray_create(bitarray->bit_count);
  if (bitarray_copy) {
    memcpy(bitarray_copy->array, bitarray->array, bitarray->array_size);
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
    array_memory_size = (size / 8) + 1;
    bitarray->array = malloc(array_memory_size);
    if (bitarray->array) {
      memset(bitarray->array, '\0', array_memory_size);
      bitarray->array_size = array_memory_size;
      bitarray->bit_count = size;
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
  assert(bitarray);
  xt_core_bitarray_array_position_t array_position;
  xt_core_bit_t bit;
  unsigned char *c;

  calculate_array_postion(index, &array_position);
  c = bitarray->array + array_position.char_index;
  bit = get_bit_from_unsigned_char(c, array_position.char_bit_index);

  return bit;
}

unsigned long xt_core_bitarray_get_size(xt_core_bitarray_t *bitarray)
{
  return bitarray->bit_count;
}

void xt_core_bitarray_set_all(xt_core_bitarray_t *bitarray)
{
  memset(bitarray->array, 255, bitarray->array_size);
}

void xt_core_bitarray_set_bit_actual(xt_core_bitarray_t *bitarray,
    unsigned long index, xt_core_bit_t value)
{
  assert(bitarray);
  xt_core_bitarray_array_position_t array_position;
  unsigned char *c;

  calculate_array_postion(index, &array_position);
  c = bitarray->array + array_position.char_index;
  set_bit_in_unsigned_char(c, array_position.char_bit_index, value);
}

unsigned long xt_core_bitarray_get_memory_size_bytes
(xt_core_bitarray_t *bitarray)
{
  assert(bitarray);
  unsigned long size;

  size = sizeof *bitarray;
  size += (bitarray->array_size / 8) + 1;

  return size;
}

void xt_core_bitarray_unset_all(xt_core_bitarray_t *bitarray)
{
  memset(bitarray->array, 0, bitarray->array_size);
}
