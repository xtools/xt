#ifndef xt_case_set_h
#define xt_case_set_h

#include "xt/case/list.h"
#include "xt/core/declarations.h"
#include "xt/core/message.h"
#include "xt/core/object.h"

xt_core_bool_t xt_case_set_absorb(xt_case_set_t *set,
    xt_case_set_t *absorb_these);

xt_core_bool_t xt_case_set_absorb_array(xt_case_set_t *set,
    xt_case_array_t *absorb_these);

xt_core_bool_t xt_case_set_absorb_copy(xt_case_set_t *set,
    xt_case_set_t *absorb_these);

xt_core_bool_t xt_case_set_absorb_list_copy(xt_case_set_t *set,
    xt_case_list_t *absorb_these);

xt_core_bool_t xt_case_set_add(xt_case_set_t *set, void *object);

xt_core_bool_t xt_case_set_add_replace(xt_case_set_t *set, void *object);

xt_core_bool_t xt_case_set_add_to_message(xt_case_set_t *set,
    xt_core_message_t *message,
    xt_core_message_add_to_message_f add_to_message);

void xt_case_set_clear(xt_case_set_t *set);

int xt_case_set_compare(void *set_object_a, void *set_object_b);

xt_core_bool_t xt_case_set_contains(xt_case_set_t *superset,
    xt_case_set_t *subset);

void *xt_case_set_copy(void *set_object);

xt_case_set_t *xt_case_set_create(xt_core_iobject_t *iobject);

xt_case_set_t *xt_case_set_create_from_message
(xt_core_iobject_t *iobject, xt_core_message_t *message,
    xt_core_message_create_from_message_f create_from_message);

xt_case_set_t *xt_case_set_create_from_union(xt_case_set_t *set_a,
    xt_case_set_t *set_b);

void xt_case_set_destroy(void *set_object);

/*
  rename as a verb?
*/
xt_case_set_t *xt_case_set_difference(xt_case_set_t *set_a,
    xt_case_set_t *set_b);

void xt_case_set_dont_destroy_objects(xt_case_set_t *set);

xt_core_bool_t xt_case_set_expunge(xt_case_set_t *set,
    xt_case_set_t *expunge_these);

void *xt_case_set_find(xt_case_set_t *set, void *decoy_object);

void *xt_case_set_find_any(xt_case_set_t *set);

void *xt_case_set_find_copy(xt_case_set_t *set, void *decoy_object);

void *xt_case_set_find_take(xt_case_set_t *set, void *decoy_object);

xt_case_array_t *xt_case_set_get_as_array(xt_case_set_t *set);

xt_case_list_t *xt_case_set_get_as_list(xt_case_set_t *set);

char *xt_case_set_get_as_delimited_string(xt_case_set_t *set,
    xt_core_object_get_as_string_f get_as_string, const char *delimiter);

xt_core_object_compare_f xt_case_set_get_compare_function
(xt_case_set_t *set);

xt_core_object_copy_f xt_case_set_get_copy_function(xt_case_set_t *set);

xt_core_object_destroy_f xt_case_set_get_destroy_function
(xt_case_set_t *set);

xt_core_iobject_t *xt_case_set_get_iobject(xt_case_set_t *set);

unsigned long xt_case_set_get_size(xt_case_set_t *set);

void *xt_case_set_iterate_next(xt_case_set_t *set);

void xt_case_set_iterate_remove(xt_case_set_t *set);

void xt_case_set_iterate_start(xt_case_set_t *set);

void xt_case_set_lock(xt_case_set_t *set);

xt_core_bool_t xt_case_set_overlaps(xt_case_set_t *set_a,
    xt_case_set_t *set_b);

void xt_case_set_print(xt_case_set_t *set,
    xt_core_object_get_as_string_f get_as_string);

xt_core_bool_t xt_case_set_remove(xt_case_set_t *set, void *object);

void xt_case_set_unlock(xt_case_set_t *set);

#endif
