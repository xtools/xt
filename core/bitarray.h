#ifndef xt_core_bitarray_h
#define xt_core_bitarray_h

#include "xt/core/bit.h"
#include "xt/core/bool.h"
#include "xt/core/message.h"
#include "xt/core/object.h"
#include "xt/core/string.h"

#define XT_CORE_BITARRAY_BITS_IN_CHAR 8
#define XT_CORE_BITARRAY_BITS_IN_LONG 32
#define XT_CORE_BITARRAY_BITS_IN_SHORT 16
#define XT_CORE_BITARRAY_BITS_IN_UNSIGNED_CHAR 8
#define XT_CORE_BITARRAY_BITS_IN_UNSIGNED_LONG 32
#define XT_CORE_BITARRAY_BITS_IN_UNSIGNED_SHORT 16

struct xt_core_bitarray_t;
typedef struct xt_core_bitarray_t xt_core_bitarray_t;

xt_core_bool_t xt_core_bitarray_add_to_message(void *bitarray_object,
    xt_core_message_t *message);

char *xt_core_bitarray_as_binary_string(void *bitarray_object);

int xt_core_bitarray_compare(void *bitarray_object_a, void *bitarray_object_b);

void *xt_core_bitarray_copy(void *bitarray_object);

xt_core_bitarray_t *xt_core_bitarray_create(unsigned long size);

xt_core_bitarray_t *xt_core_bitarray_create_from_char(char value);

xt_core_bitarray_t *xt_core_bitarray_create_from_char_bits(char value,
    unsigned short bits);

xt_core_bitarray_t *xt_core_bitarray_create_from_double(double value);

xt_core_bitarray_t *xt_core_bitarray_create_from_double_bits(double value,
    unsigned short integral_bits, unsigned short fractional_bits);

xt_core_bitarray_t *xt_core_bitarray_create_from_long(long value);

xt_core_bitarray_t *xt_core_bitarray_create_from_long_bits(long value,
    unsigned short bits);

xt_core_bitarray_t *xt_core_bitarray_create_from_message
(xt_core_message_t *message);

xt_core_bitarray_t *xt_core_bitarray_create_from_short(short value);

xt_core_bitarray_t *xt_core_bitarray_create_from_short_bits(short value,
    unsigned short bits);

xt_core_bitarray_t *xt_core_bitarray_create_from_string(char *string,
    unsigned long string_length);

xt_core_bitarray_t *xt_core_bitarray_create_from_string_bits(char *string,
    unsigned long string_length, unsigned short bits);

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_char
(unsigned char value);

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_char_bits
(unsigned char value, unsigned short bits);

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_long
(unsigned long value);

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_long_bits
(unsigned long value, unsigned short bits);

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_short
(unsigned short value);

xt_core_bitarray_t *xt_core_bitarray_create_from_unsigned_short_bits
(unsigned short value, unsigned short bits);

xt_core_bitarray_t *xt_core_bitarray_create_random(unsigned long size);

void xt_core_bitarray_destroy(void *bitarray_object);

char *xt_core_bitarray_get_as_string(void *bitarray_object);

unsigned long xt_core_bitarray_get_actual_index(xt_core_bitarray_t *bitarray,
    unsigned long virtual_index);

xt_core_bit_t xt_core_bitarray_get_bit(xt_core_bitarray_t *bitarray,
    unsigned long virtual_index);

xt_core_bool_t xt_core_bitarray_get_bool(xt_core_bitarray_t *bitarray,
    unsigned long index);

char xt_core_bitarray_get_char(xt_core_bitarray_t *bitarray,
    unsigned long index);

char xt_core_bitarray_get_char_bits(xt_core_bitarray_t *bitarray,
    unsigned long index, unsigned short bits);

double xt_core_bitarray_get_double(xt_core_bitarray_t *bitarray,
    unsigned long index);

double xt_core_bitarray_get_double_bits(xt_core_bitarray_t *bitarray,
    unsigned long index, unsigned short integral_bits,
    unsigned short fractional_bits);

/*
  TODO: deprecate in favor of xt_core_bitarray_get_double()
*/
double xt_core_bitarray_get_double_from_bits(xt_core_bitarray_t *bitarray,
    unsigned long start_index, unsigned long end_index);

long xt_core_bitarray_get_long(xt_core_bitarray_t *bitarray,
    unsigned long index);

long xt_core_bitarray_get_long_bits(xt_core_bitarray_t *bitarray,
    unsigned long index, unsigned short bits);

unsigned long xt_core_bitarray_get_memory_size_bytes
(xt_core_bitarray_t *bitarray);

short xt_core_bitarray_get_short(xt_core_bitarray_t *bitarray,
    unsigned long index);

short xt_core_bitarray_get_short_bits(xt_core_bitarray_t *bitarray,
    unsigned long index, unsigned short bits);

unsigned long xt_core_bitarray_get_size(xt_core_bitarray_t *bitarray);

char *xt_core_bitarray_get_string(xt_core_bitarray_t *bitarray,
    unsigned long index, unsigned short bits);

unsigned char xt_core_bitarray_get_unsigned_char(xt_core_bitarray_t *bitarray,
    unsigned long index);

unsigned char xt_core_bitarray_get_unsigned_char_bits
(xt_core_bitarray_t *bitarray, unsigned long index, unsigned short bits);

unsigned long xt_core_bitarray_get_unsigned_long(xt_core_bitarray_t *bitarray,
    unsigned long index);

unsigned long xt_core_bitarray_get_unsigned_long_bits
(xt_core_bitarray_t *bitarray, unsigned long index, unsigned short bits);

/*
  TODO: deprecate in favor of xt_core_bitarray_get_unsigned_long()
*/
unsigned long xt_core_bitarray_get_unsigned_long_from_bits
(xt_core_bitarray_t *bitarray, unsigned long start_index,
    unsigned long end_index);

unsigned short xt_core_bitarray_get_unsigned_short(xt_core_bitarray_t *bitarray,
    unsigned long index);

unsigned short xt_core_bitarray_get_unsigned_short_bits
(xt_core_bitarray_t *bitarray, unsigned long index, unsigned short bits);

xt_core_bool_t xt_core_bitarray_increment(xt_core_bitarray_t *bitarray);

void xt_core_bitarray_print(xt_core_bitarray_t *bitarray);

void xt_core_bitarray_randomize(xt_core_bitarray_t *bitarray);

void xt_core_bitarray_set_all(xt_core_bitarray_t *bitarray);

void xt_core_bitarray_set_bit(xt_core_bitarray_t *bitarray, unsigned long index,
    xt_core_bit_t value);

void xt_core_bitarray_set_bits_from_bitarray(xt_core_bitarray_t *destination,
    unsigned long destination_index, xt_core_bitarray_t *source,
    unsigned long source_index, unsigned long length);

void xt_core_bitarray_flip_bit(xt_core_bitarray_t *bitarray,
    unsigned long index);

void xt_core_bitarray_set_bit(xt_core_bitarray_t *bitarray,
    unsigned long virtual_index, xt_core_bit_t value);

void xt_core_bitarray_set_double(xt_core_bitarray_t *bitarray,
    unsigned long start_index, unsigned long end_index, double value);

void xt_core_bitarray_set_unsigned_long(xt_core_bitarray_t *bitarray,
    unsigned long start_index, unsigned long end_index, unsigned long value);

void xt_core_bitarray_unset_all(xt_core_bitarray_t *bitarray);

#endif
