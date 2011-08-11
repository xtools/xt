#ifndef xt_core_objects_h
#define xt_core_objects_h

#include "xt/core/iobject.h"

struct xt_core_objects_t {
  xt_core_iobject_t long_iobject;
  xt_core_iobject_t pair_iobject;
  xt_core_iobject_t string_iobject;
  xt_core_iobject_t unsigned_long_iobject;
  xt_core_iobject_t uuid_iobject;
  xt_core_iobject_t void_iobject;
};
typedef struct xt_core_objects_t xt_core_objects_t;

xt_core_objects_t xt_core_objects;

void xt_core_objects_init();

#endif
