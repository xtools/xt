#ifndef xt_core_iobject_h
#define xt_core_iobject_h

#include "xt/core/object.h"

struct xt_core_iobject_t {
  xt_core_object_compare_f compare;
  xt_core_object_compare_equal_f compare_equal;
  xt_core_object_copy_f copy;
  xt_core_object_destroy_f destroy;
  xt_core_object_get_as_string_f get_as_string;
  xt_core_object_mod_f mod;
};
typedef struct xt_core_iobject_t xt_core_iobject_t;

void xt_core_iobject_init(xt_core_iobject_t *iobject,
    xt_core_object_compare_f compare,
    xt_core_object_compare_equal_f compare_equal,
    xt_core_object_copy_f copy, xt_core_object_destroy_f destroy,
    xt_core_object_get_as_string_f get_as_string,
    xt_core_object_mod_f mod);

#endif
