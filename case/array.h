#ifndef xt_case_array_h
#define xt_case_array_h

#include "xt/case/declarations.h"
#include "xt/core/bool.h"
#include "xt/core/message.h"
#include "xt/core/object.h"

void xt_case_array_add(xt_case_array_t *array, unsigned long index,
    void *object);

xt_core_bool_t xt_case_array_add_to_message(xt_case_array_t *array,
    xt_core_message_t *message,
    xt_core_message_add_to_message_f add_to_message);

void xt_case_array_clear(xt_case_array_t *array);

int xt_case_array_compare(void *array_object_a, void *array_object_b);

void *xt_case_array_copy(void *array_object);

unsigned long xt_case_array_count_non_null_objects
(xt_case_array_t *array);

xt_case_array_t *xt_case_array_create(unsigned long initial_size,
    xt_core_object_compare_f compare, xt_core_object_copy_f copy, xt_core_object_destroy_f destroy);

xt_case_array_t *xt_case_array_create_from_message
(xt_core_object_compare_f compare_object, xt_core_object_copy_f copy,
    xt_core_object_destroy_f destroy, xt_core_message_t *message,
    xt_core_message_create_from_message_f create_from_message);

xt_case_array_t *xt_case_array_create_strings_from_string
(char *string, char *separators);

void xt_case_array_destroy(void *array_object);

void xt_case_array_expunge_set(xt_case_array_t *array,
    xt_case_set_t *expunge_these);

void xt_case_array_expunge_set_inverse(xt_case_array_t *array,
    xt_case_set_t *expunge_all_but_these);

void *xt_case_array_find(xt_case_array_t *array,
    unsigned long index);

void *xt_case_array_find_random(xt_case_array_t *array);

void *xt_case_array_find_first(xt_case_array_t *array);

void *xt_case_array_find_last(xt_case_array_t *array);

void **xt_case_array_get_array(xt_case_array_t *array);

char *xt_case_array_get_as_delimited_string(xt_case_array_t *array,
    xt_core_object_get_as_string_f get_as_string, char *delimiter);

xt_core_object_compare_f xt_case_array_get_compare(xt_case_array_t *array);

xt_core_object_copy_f xt_case_array_get_copy(xt_case_array_t *array);

xt_core_object_destroy_f xt_case_array_get_destroy(xt_case_array_t *array);

unsigned long xt_case_array_get_size(xt_case_array_t *array);

void *xt_case_array_iterate_next(xt_case_array_t *array);

void xt_case_array_iterate_remove(xt_case_array_t *array);

void xt_case_array_iterate_start(xt_case_array_t *array);

void xt_case_array_print(xt_case_array_t *array);

void xt_case_array_remove(xt_case_array_t *array,
    unsigned long index);

void xt_case_array_replace(xt_case_array_t *array,
    unsigned long index, void *object);

void xt_case_array_replace_random(xt_case_array_t *array,
    void *object);

xt_core_bool_t xt_case_array_resize(xt_case_array_t *array,
    unsigned long new_size);

void xt_case_array_sort(xt_case_array_t *array);

#endif
