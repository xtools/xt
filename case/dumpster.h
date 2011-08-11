#ifndef xt_case_dumpster_h
#define xt_case_dumpster_h

#include "xt/case/array.h"
#include "xt/case/list.h"
#include "xt/case/set.h"
#include "xt/core/iobject.h"

struct xt_case_dumpster_t;
typedef struct xt_case_dumpster_t xt_case_dumpster_t;

xt_core_bool_t xt_case_dumpster_add(xt_case_dumpster_t *dumpster,
    void *object);

xt_case_dumpster_t *xt_case_dumpster_create
(xt_core_iobject_t *iobject);

void xt_case_dumpster_destroy(xt_case_dumpster_t *dumpster);

xt_core_bool_t xt_case_dumpster_take_objects_from_list
(xt_case_dumpster_t *dumpster, xt_case_list_t *list);

#endif
