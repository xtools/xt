#include "xt/core/standard.h"
#include "xt/core/iobject.h"

void xt_core_iobject_init(xt_core_iobject_t *iobject,
    xt_core_object_compare_f compare,
    xt_core_object_compare_equal_f compare_equal,
    xt_core_object_copy_f copy, xt_core_object_destroy_f destroy,
    xt_core_object_get_as_string_f get_as_string, xt_core_object_mod_f mod)
{
  assert(iobject);

  iobject->compare = compare;
  iobject->compare_equal = compare_equal;
  iobject->copy = copy;
  iobject->destroy = destroy;
  iobject->get_as_string = get_as_string;
  iobject->mod = mod;
}
