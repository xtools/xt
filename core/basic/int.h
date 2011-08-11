#ifndef xt_core_basic_int_h
#define xt_core_basic_int_h

#include "xt/core/object.h"
#include "xt/core/iobject.h"

struct xt_core_basic_int_t;
typedef struct xt_core_basic_int_t xt_core_basic_int_t;

int xt_core_basic_int_compare(void *int_object_a, void *int_object_b);

void *xt_core_basic_int_copy(void *int_object);

xt_core_basic_int_t *xt_core_basic_int_create();

void xt_core_basic_int_destroy(void *int_object);

char *xt_core_basic_int_get_as_string(void *int_object);

void xt_core_basic_int_init_iobject(xt_core_iobject_t *iobject);

#endif
