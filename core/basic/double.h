#ifndef xt_core_basic_double_h
#define xt_core_basic_double_h

#include "xt/core/object.h"

xt_core_bool_t xt_core_basic_double_add_to_message(void *double_object,
    xt_core_message_t *message);

int xt_core_basic_double_compare(void *double_object_a, void *double_object_b);

void *xt_core_basic_double_copy(void *double_object);

void *xt_core_basic_double_create_from_message(xt_core_message_t *message);

void xt_core_basic_double_destroy(void *double_object);

#endif
