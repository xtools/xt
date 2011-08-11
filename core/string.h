#ifndef xt_core_string_h
#define xt_core_string_h

#include "xt/core/object.h"
#include "xt/core/iobject.h"

typedef char * xt_core_string_t;

xt_core_bool_t xt_core_string_add_to_message(void *string_object,
    xt_core_message_t *message);

int xt_core_string_compare(void *string_a_object, void *string_b_object);

xt_core_bool_t xt_core_string_compare_equal(void *string_a_object,
    void *string_b_object);

void *xt_core_string_copy(void *string_object);

void *xt_core_string_create_from_message(xt_core_message_t *message);

void xt_core_string_destroy(void *string_object);

char *xt_core_string_get_as_string(void *string_object);

unsigned long xt_core_string_hash(void *string_object);

void xt_core_string_init_iobject(xt_core_iobject_t *iobject);

unsigned long xt_core_string_mod(void *string_object, unsigned long divisor);

void xt_core_string_print(void *string_object);

xt_core_string_t xt_core_string_substring(xt_core_string_t string,
    unsigned long start, unsigned long length);

#endif
