#include "xt/core/bitarray.h"
#include "xt/core/tools.h"

#if defined XT_CORE_BITARRAY_COMPRESSED_IMPL
#include "xt/core/bitarray.impl.compressed.c"
#elif defined XT_CORE_BITARRAY_EXPANDED_IMPL
#include "xt/core/bitarray.impl.expanded.c"
#else
#include "xt/core/bitarray.impl.expanded.c"
#endif

#define UNSIGNED_CHAR_MAX_PLACE_VALUE 128
#define UNSIGNED_LONG_MAX_PLACE_VALUE 2147483648UL
#define UNSIGNED_SHORT_MAX_PLACE_VALUE 32768

#define SIGNED_CHAR_MAX_PLACE_VALUE 64
#define SIGNED_LONG_MAX_PLACE_VALUE 1073741824
#define SIGNED_SHORT_MAX_PLACE_VALUE 16384

static xt_core_bitarray_t *create_from_signed_long(long value,
    unsigned short bits, unsigned long max_place_value);

static xt_core_bitarray_t *create_from_unsigned_long
(unsigned long value, unsigned short bits, unsigned long max_place_value);

static long get_signed_long(xt_core_bitarray_t *bitarray, unsigned long index,
    unsigned short bits, unsigned long max_place_value);

static unsigned long get_unsigned_long(xt_core_bitarray_t *bitarray,
    unsigned long index, unsigned short bits, unsigned long max_place_value);

static xt_core_bool_t increment(xt_core_bitarray_t *bitarray,
    unsigned long position);

xt_core_bitarray_t *create_from_signed_long(long value, unsigned short bits,
    unsigned long max_place_value)
{
  assert(bits >= 2);
  xt_core_bool_t negative;
  xt_core_bitarray_t *bitarray;
  unsigned long place_value;
  unsigned long each_bit;
  unsigned long div;

  bitarray = xt_core_bitarray_create(bits);
  if (bitarray) {

    if (value < 0) {
      negative = xt_core_bool_true;
    } else {
      negative = xt_core_bool_false;
    }
    xt_core_bitarray_set_bit(bitarray, 0, negative);
    value = labs(value);

    place_value = max_place_value;
    for (each_bit = 1; each_bit < bits; each_bit++) {
      div = value / place_value;
      if (1 == div) {
        xt_core_bitarray_set_bit(bitarray, each_bit, 1);
      }
      value = value % place_value;
      place_value /= 2;
    }

  } else {
    xt_core_trace("x_core_bitarray_create");
  }

  return bitarray;
}

xt_core_bitarray_t *create_from_unsigned_long(unsigned long value,
    unsigned short bits, unsigned long max_place_value)
{
  assert(bits >= 2);
  xt_core_bitarray_t *bitarray;
  unsigned long place_value;
  unsigned long each_bit;
  unsigned long div;

  bitarray = xt_core_bitarray_create(bits);
  if (bitarray) {
    place_value = max_place_value;
    for (each_bit = 0; each_bit < bits; each_bit++) {
      div = value / place_value;
      if (1 == div) {
        xt_core_bitarray_set_bit(bitarray, each_bit, 1);
      }
      value = value % place_value;
      place_value /= 2;
      if (0 == place_value) {
        break;
      }
    }
  } else {
    xt_core_trace("x_core_bitarray_create");
  }

  return bitarray;
}

long get_signed_long(xt_core_bitarray_t *bitarray, unsigned long index,
    unsigned short bits, unsigned long max_place_value)
{
  assert(bitarray);
  assert(bits >= 2);
  assert(xt_core_bitarray_get_size(bitarray) >= 2);
  assert((index + bits) <= xt_core_bitarray_get_size(bitarray));
  long value;
  unsigned short each_bit;
  unsigned long place_value;
  xt_core_bit_t negative;
  xt_core_bit_t bit;

  negative = xt_core_bitarray_get_bit(bitarray, index);

  value = 0;
  place_value = max_place_value;
  for (each_bit = 1; each_bit < bits; each_bit++) {
    bit = xt_core_bitarray_get_bit(bitarray, index + each_bit);
    if (bit) {
      value += place_value;
    }
    place_value /= 2;
  }

  if (negative) {
    value *= -1;
  }

  return value;
}

unsigned long get_unsigned_long(xt_core_bitarray_t *bitarray,
    unsigned long index, unsigned short bits, unsigned long max_place_value)
{
  assert(bitarray);
  assert(bits >= 2);
  assert(xt_core_bitarray_get_size(bitarray) >= 2);
  unsigned long value;
  unsigned short each_bit;
  unsigned long place_value;
  xt_core_bit_t bit;

  value = 0;
  place_value = max_place_value;
  for (each_bit = 0; each_bit < bits; each_bit++) {
    bit = xt_core_bitarray_get_bit(bitarray, index + each_bit);
    if (bit) {
      value += place_value;
    }
    place_value /= 2;
  }

  return value;
}

xt_core_bool_t xt_core_bitarray_add_to_message(void *bitarray_object,
    xt_core_message_t *message)
{
  assert(bitarray_object);
  assert(message);
  xt_core_bool_t success;
  char *bitarray_as_string;
  xt_core_bitarray_t *bitarray;

  bitarray = bitarray_object;

  bitarray_as_string = xt_core_bitarray_get_as_string(bitarray);
  if (bitarray_as_string) {
    if (xt_core_message_add_string(message, bitarray_as_string)) {
      success = xt_core_bool_true;
    } else {
      success = xt_core_bool_false;
      xt_core_trace("x_core_message_add_string");
    }
    free(bitarray_as_string);
  } else {
    success = xt_core_bool_false;
    xt_core_trace("x_core_bitarray_get_as_string");
  }

  return success;
}

char *xt_core_bitarray_as_binary_string(void *bitarray_object)
{
  assert(bitarray_object);
  xt_core_bitarray_t *bitarray;
  char *string;
  unsigned long index;
  xt_core_bit_t bit;
  unsigned long array_size;

  bitarray = bitarray_object;
  array_size = xt_core_bitarray_get_size(bitarray);

  string = malloc(array_size + 1);
  if (string) {
    for (index = 0; index < array_size; index++) {
      bit = xt_core_bitarray_get_bit(bitarray, index);
      if (0 == bit) {
        *(string + index) = '0';
      } else {
        *(string + index) = '1';
      }
      *(string + array_size) = '\0';
    }
  } else {
    xt_core_trace("malloc");
  }

  return string;
}

xt_core_bitarray_t *xt_core_bitarray_create_from_char(char value)
{
  return xt_core_bitarray_create_from_char_bits(value,
      XT_CORE_BITARRAY_BITS_IN_CHAR);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_char_bits(char value,
    unsigned short bits)
{
  return create_from_signed_long(value, bits, SIGNED_CHAR_MAX_PLACE_VALUE);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_double(double value)
{
  assert(value < 65536.0);
  return xt_core_bitarray_create_from_double_bits(value, 16, 16);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_double_bits(double value,
    unsigned short integral_bits, unsigned short fractional_bits)
{
  xt_core_bitarray_t *bitarray;
  xt_core_bool_t negative;
  double integral_part;
  double fractional_part;
  unsigned long integral_long;
  unsigned long fractional_long;
  xt_core_bitarray_t *integral_bitarray;
  xt_core_bitarray_t *fractional_bitarray;

  bitarray = xt_core_bitarray_create(1 + integral_bits + fractional_bits);
  if (bitarray) {

    if (value < 0) {
      negative = xt_core_bool_true;
    } else {
      negative = xt_core_bool_false;
    }
    xt_core_bitarray_set_bit(bitarray, 0, negative);
    value = fabs(value);

    fractional_part = modf(value, &integral_part);
    fractional_part *= 100000;
    integral_long = (unsigned long) integral_part;
    fractional_long = (unsigned long) fractional_part;

    integral_bitarray = create_from_unsigned_long
      (integral_long, 16, 32768);
    if (integral_bitarray) {
      xt_core_bitarray_set_bits_from_bitarray
        (bitarray, 1, integral_bitarray, 0, 16);
      xt_core_bitarray_destroy(integral_bitarray);
    } else {
      xt_core_trace("create_from_unsigned_long");
    }

    fractional_bitarray = create_from_unsigned_long
      (fractional_long, 16, 32768);
    if (fractional_bitarray) {
      xt_core_bitarray_set_bits_from_bitarray
        (bitarray, 17, fractional_bitarray, 0, 16);
      xt_core_bitarray_destroy(fractional_bitarray);
    } else {
      xt_core_trace("create_from_unsigned_long");
    }

  } else {
    xt_core_trace("x_core_bitarray_create");
  }

  return bitarray;
}

xt_core_bitarray_t *xt_core_bitarray_create_from_long(long value)
{
  return xt_core_bitarray_create_from_long_bits
    (value, XT_CORE_BITARRAY_BITS_IN_LONG);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_long_bits(long value,
    unsigned short bits)
{
  return create_from_signed_long(value, bits, SIGNED_LONG_MAX_PLACE_VALUE);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_message
(xt_core_message_t *message)
{
  assert(message);
  xt_core_bitarray_t *bitarray;
  char *string;
  unsigned long string_length;
  unsigned long each_bit;
  char bit_char;
  xt_core_bit_t bit;

  string = xt_core_message_take_string(message);
  if (string) {
    string_length = strlen(string);
    bitarray = xt_core_bitarray_create(string_length);
    if (bitarray) {
      for (each_bit = 0; each_bit < string_length; each_bit++) {
        bit_char = *(string + each_bit);
        if ('0' == bit_char) {
          bit = 0;
        } else {
          bit = 1;
        }
        xt_core_bitarray_set_bit(bitarray, each_bit, bit);
      }
    } else {
      xt_core_trace("x_core_bitarray_create");
    }
    free(string);
  } else {
    xt_core_trace("x_core_message_take_string");
    bitarray = NULL;
  }

  return bitarray;
}

xt_core_bitarray_t *xt_core_bitarray_create_from_short(short value)
{
  return xt_core_bitarray_create_from_short_bits
    (value, XT_CORE_BITARRAY_BITS_IN_SHORT);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_short_bits(short value,
    unsigned short bits)
{
  return create_from_signed_long(value, bits, SIGNED_SHORT_MAX_PLACE_VALUE);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_string(char *string,
    unsigned long string_length)
{
  return xt_core_bitarray_create_from_string_bits(string, string_length,
      string_length * XT_CORE_BITARRAY_BITS_IN_CHAR);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_string_bits(char *string,
    unsigned long string_length, unsigned short bits)
{
  assert(string);
  unsigned short each_char;
  unsigned char c;
  xt_core_bitarray_t *bitarray;
  xt_core_bitarray_t *char_bitarray;
  unsigned short bit_position;
  unsigned short char_count;

  bitarray = xt_core_bitarray_create(bits);
  if (bitarray) {
    char_count = bits / XT_CORE_BITARRAY_BITS_IN_CHAR;
    for (each_char = 0; each_char < char_count; each_char++) {
      c = *(string + each_char);
      char_bitarray = xt_core_bitarray_create_from_unsigned_char(c);
      if (char_bitarray) {
        bit_position = each_char * XT_CORE_BITARRAY_BITS_IN_CHAR;
        xt_core_bitarray_set_bits_from_bitarray(bitarray, bit_position,
            char_bitarray, 0, XT_CORE_BITARRAY_BITS_IN_CHAR);
        xt_core_bitarray_destroy(char_bitarray);
      } else {
        xt_core_trace("x_core_bitarray_create_from_unsigned_char");
      }
    }
  } else {
    xt_core_trace("x_core_bitarray_create");
  }

  return bitarray;
}

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_char
(unsigned char value)
{
  return xt_core_bitarray_create_from_unsigned_char_bits
    (value, XT_CORE_BITARRAY_BITS_IN_UNSIGNED_CHAR);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_char_bits
(unsigned char value, unsigned short bits)
{
  return create_from_unsigned_long(value, bits, UNSIGNED_CHAR_MAX_PLACE_VALUE);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_long
(unsigned long value)
{
  assert(value <= 2147483647);
  return xt_core_bitarray_create_from_unsigned_long_bits
    (value, XT_CORE_BITARRAY_BITS_IN_UNSIGNED_LONG);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_long_bits
(unsigned long value, unsigned short bits)
{
  return create_from_unsigned_long(value, bits, UNSIGNED_LONG_MAX_PLACE_VALUE);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_short
(unsigned short value)
{
  return xt_core_bitarray_create_from_unsigned_short_bits
    (value, XT_CORE_BITARRAY_BITS_IN_UNSIGNED_SHORT);
}

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_short_bits
(unsigned short value, unsigned short bits)
{
  return xt_core_bitarray_create_from_short_bits((short) value, bits);
}

xt_core_bitarray_t *xt_core_bitarray_create_random(unsigned long size)
{
  xt_core_bitarray_t *bitarray;

  bitarray = xt_core_bitarray_create(size);
  if (bitarray) {
    xt_core_bitarray_randomize(bitarray);
  } else {
    xt_core_trace("x_core_bitarray_create");
  }

  return bitarray;
}

char *xt_core_bitarray_get_as_string(void *bitarray_object)
{
  assert(bitarray_object);
  xt_core_bitarray_t *bitarray;
  char *string;
  unsigned long bitarray_size;
  unsigned long each_bit;
  unsigned long char_index;
  unsigned long bit_index;
  xt_core_bit_t bit;
  char *c;
  unsigned long string_size;

  bitarray = bitarray_object;

  bitarray_size = xt_core_bitarray_get_size(bitarray);

  /*
    for every bit in the bitarray, plus 1 terminator bit, we'll need 2 actual
    bits for the encoded string, plus 1 extra byte at the end in case the
    "/ 8" had a remainder, plus 1 more byte for the terminator
  */
  string_size = (((bitarray_size + 1) * 2) / 8) + 1 + 1;

  string = malloc(string_size);
  if (string) {
    memset(string, 0, string_size);
    for (each_bit = 0; each_bit < bitarray_size; each_bit++) {

      char_index = (each_bit * 2) / 8;
      bit_index = (each_bit * 2) % 8;

      c = string + char_index;

      bit = xt_core_bitarray_get_bit(bitarray, each_bit);
      if (bit) {
        xt_core_set_bit_in_unsigned_char((unsigned char *) c, bit_index, 1);
        xt_core_set_bit_in_unsigned_char((unsigned char *) c, bit_index + 1, 1);
      } else {
        xt_core_set_bit_in_unsigned_char((unsigned char *) c, bit_index, 0);
        xt_core_set_bit_in_unsigned_char((unsigned char *) c, bit_index + 1, 1);
      }

    }
  } else {
    xt_core_trace("malloc");
  }

  return string;
}

xt_core_bit_t xt_core_bitarray_get_bit(xt_core_bitarray_t *bitarray,
    unsigned long virtual_index)
{
  assert(bitarray);
  unsigned long index;
  xt_core_bit_t bit;

  index = xt_core_wrap_index(virtual_index, bitarray->array_size);
  bit = xt_core_bitarray_get_bit_actual(bitarray, index);

  return bit;
}

unsigned long xt_core_bitarray_get_actual_index(xt_core_bitarray_t *bitarray,
    unsigned long virtual_index)
{
  return xt_core_wrap_index(virtual_index, bitarray->array_size);
}

xt_core_bool_t xt_core_bitarray_get_bool(xt_core_bitarray_t *bitarray,
    unsigned long index)
{
  return xt_core_bitarray_get_bit(bitarray, index);
}

char xt_core_bitarray_get_char(xt_core_bitarray_t *bitarray, unsigned long index)
{
  return xt_core_bitarray_get_char_bits(bitarray, index,
      XT_CORE_BITARRAY_BITS_IN_CHAR);
}

char xt_core_bitarray_get_char_bits(xt_core_bitarray_t *bitarray,
    unsigned long index, unsigned short bits)
{
  return get_signed_long(bitarray, index, bits, SIGNED_CHAR_MAX_PLACE_VALUE);
}

double xt_core_bitarray_get_double(xt_core_bitarray_t *bitarray,
    unsigned long index)
{
  return xt_core_bitarray_get_double_bits(bitarray, index, 16, 16);
}

double xt_core_bitarray_get_double_bits(xt_core_bitarray_t *bitarray,
    unsigned long index,
    unsigned short integral_bits,
    unsigned short fractional_bits)
{
  assert(bitarray);
  assert(integral_bits >= 1 && integral_bits <= 16);
  assert(fractional_bits >= 1 && fractional_bits <= 16);
  assert(xt_core_bitarray_get_size(bitarray) >= 2);
  assert((index + 1 + integral_bits + fractional_bits)
      <= xt_core_bitarray_get_size(bitarray));
  double value;
  xt_core_bit_t negative;
  unsigned long integral_long;
  unsigned long fractional_long;
  double fractional_value;

  negative = xt_core_bitarray_get_bit(bitarray, index);

  integral_long = get_unsigned_long(bitarray, index + 1, integral_bits,
      pow(2, integral_bits - 1));

  fractional_long = get_unsigned_long(bitarray, index + 1 + integral_bits,
      fractional_bits, pow(2, fractional_bits - 1));
  fractional_value = fractional_long / 100000.0;

  value = integral_long + fractional_value;

  if (negative) {
    value *= -1;
  }

  return value;
}

double xt_core_bitarray_get_double_from_bits(xt_core_bitarray_t *bitarray,
    unsigned long start_index, unsigned long end_index)
{
  assert(bitarray);
  assert(start_index >= 0);
  assert(end_index < xt_core_bitarray_get_size(bitarray));
  assert(start_index <= end_index);
  double dividend;
  double divisor;
  double result;

  dividend = (double) xt_core_bitarray_get_unsigned_long_from_bits(bitarray,
      start_index, end_index);
  divisor = pow(2.0, ((double) end_index - (double) start_index + 1.0)) - 1.0;
  if (0 == divisor) {
    result = 0.0;
  } else {
    result = dividend / (double) divisor;
  }

  return result;
}

long xt_core_bitarray_get_long(xt_core_bitarray_t *bitarray, unsigned long index)
{
  return xt_core_bitarray_get_long_bits(bitarray, index,
      XT_CORE_BITARRAY_BITS_IN_LONG);
}

long xt_core_bitarray_get_long_bits(xt_core_bitarray_t *bitarray,
    unsigned long index, unsigned short bits)
{
  return get_signed_long(bitarray, index, bits, SIGNED_LONG_MAX_PLACE_VALUE);
}

short xt_core_bitarray_get_short(xt_core_bitarray_t *bitarray,
    unsigned long index)
{
  return xt_core_bitarray_get_short_bits(bitarray, index,
      XT_CORE_BITARRAY_BITS_IN_SHORT);
}

short xt_core_bitarray_get_short_bits(xt_core_bitarray_t *bitarray,
    unsigned long index, unsigned short bits)
{
  return get_signed_long(bitarray, index, bits, SIGNED_SHORT_MAX_PLACE_VALUE);
}

char *xt_core_bitarray_get_string(xt_core_bitarray_t *bitarray,
    unsigned long index, unsigned short bits)
{
  assert(bitarray);
  char *string;
  unsigned long string_length;
  unsigned long each_char;
  unsigned long start_bit;
  unsigned char c;

  string_length = bits / XT_CORE_BITARRAY_BITS_IN_CHAR;

  string = malloc(string_length + 1);
  if (string) {
    for (each_char = 0; each_char < string_length; each_char++) {
      start_bit = index + (each_char * XT_CORE_BITARRAY_BITS_IN_CHAR);
      c = xt_core_bitarray_get_unsigned_char(bitarray, start_bit);
      *(string + each_char) = c;
    }
    *(string + string_length) = '\0';
  } else {
    xt_core_trace("malloc");
  }

  return string;
}

unsigned char xt_core_bitarray_get_unsigned_char(xt_core_bitarray_t *bitarray,
    unsigned long index)
{
  return xt_core_bitarray_get_unsigned_char_bits(bitarray, index,
      XT_CORE_BITARRAY_BITS_IN_UNSIGNED_CHAR);
}

unsigned char xt_core_bitarray_get_unsigned_char_bits
(xt_core_bitarray_t *bitarray, unsigned long index, unsigned short bits)
{
  return get_unsigned_long(bitarray, index, bits,
      UNSIGNED_CHAR_MAX_PLACE_VALUE);
}

unsigned long xt_core_bitarray_get_unsigned_long(xt_core_bitarray_t *bitarray,
    unsigned long index)
{
  return xt_core_bitarray_get_unsigned_long_bits(bitarray, index,
      XT_CORE_BITARRAY_BITS_IN_UNSIGNED_CHAR);
}

unsigned long xt_core_bitarray_get_unsigned_long_bits
(xt_core_bitarray_t *bitarray, unsigned long index, unsigned short bits)
{
  return get_unsigned_long(bitarray, index, bits,
      UNSIGNED_LONG_MAX_PLACE_VALUE);
}

unsigned long xt_core_bitarray_get_unsigned_long_from_bits
(xt_core_bitarray_t *bitarray, unsigned long start_index,
    unsigned long end_index)
{
  assert(bitarray);
  assert(start_index >= 0);
  assert(end_index < xt_core_bitarray_get_size(bitarray));
  assert(start_index <= end_index);

  unsigned long value;
  unsigned long place_value;
  unsigned long place;
  unsigned long overplace;
  xt_core_bit_t bit;

  value = 0;
  place_value = 1;
  overplace = start_index + (end_index - start_index);

  for (place = start_index; place <= overplace; place++) {
    bit = xt_core_bitarray_get_bit(bitarray, place);
    value += (bit * place_value);
    place_value *= 2;
  }

  return value;
}

unsigned short xt_core_bitarray_get_unsigned_short(xt_core_bitarray_t *bitarray,
    unsigned long index)
{
  return xt_core_bitarray_get_unsigned_short_bits(bitarray, index,
      XT_CORE_BITARRAY_BITS_IN_UNSIGNED_SHORT);
}

unsigned short xt_core_bitarray_get_unsigned_short_bits
(xt_core_bitarray_t *bitarray, unsigned long index, unsigned short bits)
{
  return get_unsigned_long(bitarray, index, bits,
      UNSIGNED_SHORT_MAX_PLACE_VALUE);
}

xt_core_bool_t xt_core_bitarray_increment(xt_core_bitarray_t *bitarray)
{
  return increment(bitarray, 0);
}

void xt_core_bitarray_print(xt_core_bitarray_t *bitarray)
{
  xt_core_object_print(bitarray, xt_core_bitarray_as_binary_string);
}

void xt_core_bitarray_randomize(xt_core_bitarray_t *bitarray)
{
  assert(bitarray);
  unsigned long index;
  xt_core_bit_t value;
  unsigned long array_size;

  array_size = xt_core_bitarray_get_size(bitarray);

  for (index = 0; index < array_size; index++) {
    value = random() % 2;
    xt_core_bitarray_set_bit(bitarray, index, value);
  }
}

void xt_core_bitarray_flip_bit(xt_core_bitarray_t *bitarray, unsigned long index)
{
  assert(bitarray);
  assert(index >= 0);
  assert(index < xt_core_bitarray_get_size(bitarray));

  if (xt_core_bitarray_get_bit(bitarray, index)) {
    xt_core_bitarray_set_bit(bitarray, index, 0);
  } else {
    xt_core_bitarray_set_bit(bitarray, index, 1);
  }
}

void xt_core_bitarray_set_bit(xt_core_bitarray_t *bitarray,
    unsigned long virtual_index, xt_core_bit_t value)
{
  assert(bitarray);
  unsigned long index;

  index = xt_core_wrap_index(virtual_index, bitarray->array_size);
  xt_core_bitarray_set_bit_actual(bitarray, index, value);
}

void xt_core_bitarray_set_bits_from_bitarray(xt_core_bitarray_t *destination,
    unsigned long destination_index, xt_core_bitarray_t *source,
    unsigned long source_index, unsigned long length)
{
  assert(destination);
  assert(source);
  assert(length > 0);
  assert((destination_index + length) <= destination->array_size);
  assert((source_index + length) <= source->array_size);
  unsigned long l;
  xt_core_bit_t bit;

  for (l = 0; l < length; l++) {
    bit = xt_core_bitarray_get_bit(source, source_index + l);
    xt_core_bitarray_set_bit(destination, destination_index + l, bit);
  }
}

/*
  TODO: reimplement to match the pair-of-unsigned-longs way of doing this
*/
void xt_core_bitarray_set_double(xt_core_bitarray_t *bitarray,
    unsigned long start_index, unsigned long end_index, double value)
{
  assert(bitarray);
  assert(start_index < end_index);
  assert(end_index < xt_core_bitarray_get_size(bitarray));
  assert(value <= 1.0);
  assert(value >= 0.0);
  unsigned long base = 0;
  unsigned long integer = 0;

  base = pow(2, (end_index - start_index + 1)) - 1;
  integer = (unsigned long) round(value * base);

  return xt_core_bitarray_set_unsigned_long
    (bitarray, start_index, end_index, integer);
}

void xt_core_bitarray_set_unsigned_long(xt_core_bitarray_t *bitarray,
    unsigned long start_index, unsigned long end_index, unsigned long value)
{
  assert(bitarray);
  assert(start_index <= end_index);
  assert(end_index < xt_core_bitarray_get_size(bitarray));
  assert(value < pow(2, ((end_index - start_index) + 1)));
  unsigned long i;
  unsigned long working_value;

  working_value = value;
  for (i = start_index; i <= end_index; i++) {

    if (working_value % 2) {
      xt_core_bitarray_set_bit(bitarray, i, 1);
    } else {
      xt_core_bitarray_set_bit(bitarray, i, 0);
    }

    working_value /= 2;
  }
}

xt_core_bool_t increment(xt_core_bitarray_t *bitarray, unsigned long position)
{
  assert(bitarray);
  xt_core_bool_t success;
  long each_bit;
  xt_core_bit_t bit;
  unsigned long array_size;

  array_size = xt_core_bitarray_get_size(bitarray);

  if (position >= array_size) {
    success = xt_core_bool_false;
  } else {
    bit = xt_core_bitarray_get_bit(bitarray, position);
    if (0 == bit) {
      success = xt_core_bool_true;
      xt_core_bitarray_set_bit(bitarray, position, 1);
      if (position > 0) {
        for (each_bit = (position - 1); each_bit >= 0; each_bit--) {
          xt_core_bitarray_set_bit(bitarray, each_bit, 0);
        }
      }
    } else {
      success = increment(bitarray, position + 1);
    }
  }

  return success;
}
