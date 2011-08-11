#ifndef xt_net_void_h
#define xt_net_void_h

#include "xt/core/iobject.h"

int xt_core_basic_void_compare(void *object_a, void *object_b);

void *xt_core_basic_void_copy(void *object);

void xt_core_basic_void_destroy(void *object);

char *xt_core_basic_void_get_as_string(void *object);

void xt_core_basic_void_init_iobject(xt_core_iobject_t *iobject);

#endif
