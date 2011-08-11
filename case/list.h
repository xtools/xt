#ifndef xt_case_list_h
#define xt_case_list_h

#include "xt/case/declarations.h"
#include "xt/core/iobject.h"
#include "xt/core/message.h"
#include "xt/core/object.h"

#define XT_CASE_LIST_NO_SIZE_LIMIT 0

struct xt_case_list_t;
typedef struct xt_case_list_t xt_case_list_t;

xt_core_bool_t xt_case_list_absorb(xt_case_list_t *list,
    xt_case_list_t *absorb_these);

xt_core_bool_t xt_case_list_add_to_message(xt_case_list_t *list,
    xt_core_message_t *message,
    xt_core_message_add_to_message_f add_to_message);

xt_core_bool_t xt_case_list_add_first(xt_case_list_t *list,
    void *object);

xt_core_bool_t xt_case_list_add_last(xt_case_list_t *list, void *object);

void xt_case_list_clear(xt_case_list_t *list);

int xt_case_list_compare(void *list_object_a, void *list_object_b);

void *xt_case_list_copy(void *list_object);

xt_case_list_t *xt_case_list_create(xt_core_object_compare_f compare,
    xt_core_object_copy_f copy, xt_core_object_destroy_f destroy);

xt_case_list_t *xt_case_list_create_from_array_n
(xt_case_array_t *array, unsigned long max_size);

xt_case_list_t *xt_case_list_create_from_message
(xt_core_object_compare_f compare, xt_core_object_copy_f copy,
    xt_core_object_destroy_f destroy, xt_core_message_t *message,
    xt_core_message_create_from_message_f create_from_message);

xt_case_list_t *xt_case_list_create_strings_from_string(char *string,
    const char *separators);

void xt_case_list_destroy(void *list_object);

void xt_case_list_dont_destroy_objects(xt_case_list_t *list);

void *xt_case_list_find_at(xt_case_list_t *list, unsigned long index);

void *xt_case_list_find_first(xt_case_list_t *list);

void *xt_case_list_find_last(xt_case_list_t *list);

void *xt_case_list_find_random(xt_case_list_t *list);

char *xt_case_list_get_as_delimited_string(xt_case_list_t *list,
    xt_core_object_get_as_string_f get_as_string, const char *delimiter);

char *xt_case_list_get_as_string(void *list_object);

unsigned long xt_case_list_get_size(xt_case_list_t *list);

void xt_case_list_init_iobject(xt_core_iobject_t *iobject);

void *xt_case_list_iterate_next(xt_case_list_t *list);

void xt_case_list_iterate_remove(xt_case_list_t *list);

void xt_case_list_iterate_start(xt_case_list_t *list);

void xt_case_list_lock(xt_case_list_t *list);

void xt_case_list_print(void *list_object);

xt_core_bool_t xt_case_list_remove_first(xt_case_list_t *list);

xt_core_bool_t xt_case_list_remove_last(xt_case_list_t *list);

void xt_case_list_set_size_limit(xt_case_list_t *list,
    unsigned long size_limit);

void xt_case_list_unlock(xt_case_list_t *list);

#endif
