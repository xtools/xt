#ifndef xt_core_basic_short_h
#define xt_core_basic_short_h

#include "xt/core/object.h"

xt_core_bool_t xt_core_basic_short_add_to_message(void *short_object,
    xt_core_message_t *message);

int xt_core_basic_short_compare(void *short_a_object, void *short_b_object);

void *xt_core_basic_short_copy(void *short_object);

void *xt_core_basic_short_take_from_message(xt_core_message_t *message);

void xt_core_basic_short_destroy(void *short_object);

void xt_core_basic_short_print(void *short_object);

#endif
