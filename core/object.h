#ifndef xt_core_object_h
#define xt_core_object_h

#include "xt/core/bool.h"

#define XT_CORE_OBJECT_NO_COMPARE_F NULL
#define XT_CORE_OBJECT_NO_COMPARE_EQUAL_F NULL
#define XT_CORE_OBJECT_NO_COPY_F NULL
#define XT_CORE_OBJECT_NO_DESTROY_F NULL
#define XT_CORE_OBJECT_NO_GET_AS_STRING_F NULL
#define XT_CORE_OBJECT_NO_MOD_F NULL

typedef int (*xt_core_object_compare_f)(void *object_a, void *object_b);

typedef xt_core_bool_t (*xt_core_object_compare_equal_f)(void *object_a,
    void *object_b);

typedef int (*xt_core_object_compare_const_f)(const void *a_object,
    const void *b_object);

typedef xt_core_bool_t (*xt_core_object_evaluate_condition_f)
(void *object);

typedef void *(*xt_core_object_copy_f)(void *object);

typedef void *(*xt_core_object_create_f)();

typedef void (*xt_core_object_destroy_f)(void *object);

typedef char *(*xt_core_object_get_as_string_f)(void *object);

typedef void *(*xt_core_object_get_object_f)(void *object);

typedef unsigned long (*xt_core_object_hash_f)(void *object);

typedef unsigned long (*xt_core_object_mod_f)(void *object,
    unsigned long divisor);

void xt_core_object_print(void *object,
    xt_core_object_get_as_string_f get_as_string);

typedef void (*xt_core_object_set_object_f)(void *object, void *value);

#endif
