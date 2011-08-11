#include "xt/case/dumpster.h"
#include "xt/core/tools.h"

struct xt_case_dumpster_t {
  xt_core_iobject_t *iobject;
  xt_case_list_t *objects;
};

xt_core_bool_t xt_case_dumpster_add(xt_case_dumpster_t *dumpster,
    void *object)
{
  assert(dumpster);
  assert(object);
  xt_core_bool_t success;

  if (xt_case_list_add_last(dumpster->objects, object)) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

xt_case_dumpster_t *xt_case_dumpster_create
(xt_core_iobject_t *iobject)
{
  assert(iobject);
  xt_case_dumpster_t *dumpster;

  dumpster = malloc(sizeof *dumpster);
  if (dumpster) {
    dumpster->iobject = iobject;
    dumpster->objects = xt_case_list_create(iobject->compare,
        iobject->copy, iobject->destroy);
    if (!dumpster->objects) {
      free(dumpster);
      dumpster = NULL;
    }
  } else {
    xt_core_trace("malloc");
  }

  return dumpster;
}

void xt_case_dumpster_destroy(xt_case_dumpster_t *dumpster)
{
  assert(dumpster);

  xt_case_list_destroy(dumpster->objects);
  free(dumpster);
}

xt_core_bool_t xt_case_dumpster_take_objects_from_list
(xt_case_dumpster_t *dumpster, xt_case_list_t *list)
{
  assert(dumpster);
  assert(list);
  xt_core_bool_t success;
  void *object;

  success = xt_core_bool_true;

  xt_case_list_dont_destroy_objects(list);
  xt_case_list_iterate_start(list);
  while ((object = xt_case_list_iterate_next(list))) {
    if (xt_case_dumpster_add(dumpster, object)) {
      xt_case_list_iterate_remove(list);
    } else {
      success = xt_core_bool_false;
      xt_core_trace("x_case_dumpster_add_object");
      break;
    }
  }

  return success;
}
