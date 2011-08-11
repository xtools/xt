#ifndef xt_file_csv_h
#define xt_file_csv_h

#include "xt/case/array.h"
#include "xt/core/bit.h"
#include "xt/core/object.h"

struct xt_file_csv_t;
typedef struct xt_file_csv_t xt_file_csv_t;

xt_file_csv_t *xt_file_csv_create(char *filename);

xt_file_csv_t *xt_file_csv_create_extended(char *filename,
    unsigned long start_object, unsigned long end_object);

void xt_file_csv_destroy(void *csv_object);

xt_case_array_t *xt_file_csv_get_field_by_name_as_array(xt_file_csv_t *csv,
    char *field_name);

xt_case_array_t *xt_file_csv_get_field_by_index_as_array(xt_file_csv_t *csv,
    unsigned long field_index);

unsigned long xt_file_csv_get_field_count(xt_file_csv_t *csv);

xt_core_bool_t xt_file_csv_get_field_index(xt_file_csv_t *csv, char *field_name,
    unsigned long *field_index);

char *xt_file_csv_get_field_name(xt_file_csv_t *csv, unsigned long field_index);

unsigned long xt_file_csv_get_object_count(xt_file_csv_t *csv);

xt_core_bit_t xt_file_csv_get_value_by_index_as_bit(xt_file_csv_t *csv,
    unsigned long object_index, unsigned long field_index);

xt_core_bool_t xt_file_csv_get_value_by_index_as_bool(xt_file_csv_t *csv,
    unsigned long object_index, unsigned long field_index);

char xt_file_csv_get_value_by_index_as_char(xt_file_csv_t *csv,
    unsigned long object_index, unsigned long field_index);

double xt_file_csv_get_value_by_index_as_double(xt_file_csv_t *csv,
    unsigned long object_index, unsigned long field_index);

long xt_file_csv_get_value_by_index_as_long(xt_file_csv_t *csv,
    unsigned long object_index, unsigned long field_index);

short xt_file_csv_get_value_by_index_as_short(xt_file_csv_t *csv,
    unsigned long object_index, unsigned long field_index);

char *xt_file_csv_get_value_by_index_as_string(xt_file_csv_t *csv,
    unsigned long object_index, unsigned long field_index);

unsigned char xt_file_csv_get_value_by_index_as_unsigned_char
(xt_file_csv_t *csv, unsigned long object_index, unsigned long field_index);

unsigned long xt_file_csv_get_value_by_index_as_unsigned_long
(xt_file_csv_t *csv, unsigned long object_index, unsigned long field_index);

unsigned short xt_file_csv_get_value_by_index_as_unsigned_short
(xt_file_csv_t *csv, unsigned long object_index, unsigned long field_index);

char xt_file_csv_get_value_by_name_as_char(xt_file_csv_t *csv,
    unsigned long object_index, char *field_name);

double xt_file_csv_get_value_by_name_as_double(xt_file_csv_t *csv,
    unsigned long object_index, char *field_name);

long xt_file_csv_get_value_by_name_as_long(xt_file_csv_t *csv,
    unsigned long object_index, char *field_name);

short xt_file_csv_get_value_by_name_as_short(xt_file_csv_t *csv,
    unsigned long object_index, char *field_name);

char *xt_file_csv_get_value_by_name_as_string(xt_file_csv_t *csv,
    unsigned long object_index, char *field_name);

unsigned char xt_file_csv_get_value_by_name_as_unsigned_char(xt_file_csv_t *csv,
    unsigned long object_index, char *field_name);

unsigned long xt_file_csv_get_value_by_name_as_unsigned_long(xt_file_csv_t *csv,
    unsigned long object_index, char *field_name);

unsigned short xt_file_csv_get_value_by_name_as_unsigned_short
(xt_file_csv_t *csv, unsigned long object_index, char *field_name);

#endif
