#ifndef xt_core_basic_unsigned_long_h
#define xt_core_basic_unsigned_long_h

#include "xt/core/iobject.h"

int xt_core_basic_unsigned_long_compare(void *unsigned_long_a_object,
    void *unsigned_long_b_object);

xt_core_bool_t xt_core_basic_unsigned_long_compare_equal
(void *unsigned_long_a_object, void *unsigned_long_b_object);

void *xt_core_basic_unsigned_long_copy(void *unsigned_long_object);

void xt_core_basic_unsigned_long_destroy(void *unsigned_long_object);

char *xt_core_basic_unsigned_long_get_as_string(void *unsigned_long_object);

void xt_core_basic_unsigned_long_init_iobject
(xt_core_iobject_t *iobject);

unsigned long xt_core_basic_unsigned_long_mod(void *unsigned_long_object,
    unsigned long divisor);

#endif
