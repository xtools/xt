#ifndef xt_core_basic_long_h
#define xt_core_basic_long_h

#include "xt/core/iobject.h"
#include "xt/core/object.h"

xt_core_bool_t xt_core_basic_long_add_to_message(void *long_object,
    xt_core_message_t *message);

int xt_core_basic_long_compare(void *long_a_object, void *long_b_object);

void *xt_core_basic_long_copy(void *long_object);

void *xt_core_basic_long_create_from_message(xt_core_message_t *message);

void xt_core_basic_long_destroy(void *long_object);

xt_core_bool_t xt_core_basic_long_compare_equal(void *long_a_object,
    void *long_b_object);

char *xt_core_basic_long_get_as_string(void *long_object);

void xt_core_basic_long_init_iobject(xt_core_iobject_t *iobject);

unsigned long xt_core_basic_long_mod(void *long_object,
    unsigned long divisor);

void xt_core_basic_long_print(void *long_object);

#endif
