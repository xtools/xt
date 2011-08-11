#ifndef xt_core_pair_h
#define xt_core_pair_h

#include "xt/core/object.h"
#include "xt/core/iobject.h"

#define XT_CORE_PAIR_DESTROYS xt_core_bool_true
#define XT_CORE_PAIR_DONT_DESTROY xt_core_bool_false

struct xt_core_pair_t;
typedef struct xt_core_pair_t xt_core_pair_t;

int xt_core_pair_compare(void *pair_object_a, void *pair_object_b);

int xt_core_pair_compare_left(void *pair_object_a, void *pair_object_b);

int xt_core_pair_compare_right(void *pair_object_a, void *pair_object_b);

void *xt_core_pair_copy(void *pair_object);

xt_core_pair_t *xt_core_pair_create(void *left, xt_core_iobject_t *left_iobject,
    void *right, xt_core_iobject_t *right_iobject, xt_core_bool_t destroys);

xt_core_pair_t *xt_core_pair_create_decoy(void *left,
    xt_core_iobject_t *left_iobject);

void xt_core_pair_destroy(void *pair_object);

void xt_core_pair_destroy_decoy(xt_core_pair_t *pair);

xt_core_bool_t xt_core_pair_equal_left(void *pair_a_object, void *pair_b_object);

char *xt_core_pair_get_as_string(void *pair_object);

void *xt_core_pair_get_left(xt_core_pair_t *pair);

xt_core_iobject_t *xt_core_pair_get_left_iobject(xt_core_pair_t *pair);

void *xt_core_pair_get_right(xt_core_pair_t *pair);

xt_core_iobject_t *xt_core_pair_get_right_iobject(xt_core_pair_t *pair);

void xt_core_pair_init_iobject(xt_core_iobject_t *iobject);

unsigned long xt_core_pair_mod_left(void *pair_object, unsigned long divisor);

#endif
