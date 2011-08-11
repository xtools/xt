#ifndef xt_case_mbin_h
#define xt_case_mbin_h

#include "xt/core/object.h"
#include "xt/core/iobject.h"
#include "xt/core/tools.h"

enum xt_case_mbin_set_type_t {
  XT_CASE_MBIN_SET_TYPE_MULTISET,
  XT_CASE_MBIN_SET_TYPE_SET
};
typedef enum xt_case_mbin_set_type_t xt_case_mbin_set_type_t;

struct xt_case_mbin_t;
typedef struct xt_case_mbin_t xt_case_mbin_t;

xt_core_bool_t xt_case_mbin_add(xt_case_mbin_t *mbin, void *object);

void xt_case_mbin_clear(xt_case_mbin_t *mbin);

xt_case_mbin_t *xt_case_mbin_create(xt_core_iobject_t *iobject,
    xt_case_mbin_set_type_t set_type);

void xt_case_mbin_destroy(xt_case_mbin_t *mbin);

void xt_case_mbin_dont_destroy_objects(xt_case_mbin_t *mbin);

void *xt_case_mbin_find(xt_case_mbin_t *mbin, void *decoy_object);

xt_core_iobject_t *xt_case_mbin_get_iobject(xt_case_mbin_t *mbin);

unsigned long xt_case_mbin_get_size(xt_case_mbin_t *mbin);

void xt_case_mbin_iterate_remove(xt_case_mbin_t *mbin);

void xt_case_mbin_iterate_start(xt_case_mbin_t *mbin);

void *xt_case_mbin_iterate_next(xt_case_mbin_t *mbin);

xt_core_bool_t xt_case_mbin_remove(xt_case_mbin_t *mbin,
    void *decoy_object);

#endif
