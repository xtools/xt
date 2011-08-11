#ifndef xt_case_hasha_h
#define xt_case_hasha_h

#include "xt/core/object.h"
#include "xt/core/iobject.h"
#include "xt/core/tools.h"

struct xt_case_hasha_t;
typedef struct xt_case_hasha_t xt_case_hasha_t;

xt_core_bool_t xt_case_hasha_add(xt_case_hasha_t *hasha, void *object);

void xt_case_hasha_clear(xt_case_hasha_t *hasha);

xt_case_hasha_t *xt_case_hasha_create(xt_core_iobject_t *iobject);

void xt_case_hasha_destroy(xt_case_hasha_t *hasha);

void xt_case_hasha_dont_destroy_objects(xt_case_hasha_t *hasha);

void *xt_case_hasha_find(xt_case_hasha_t *hasha, void *decoy_object);

xt_core_iobject_t *xt_case_hasha_get_iobject(xt_case_hasha_t *hasha);

unsigned long xt_case_hasha_get_size(xt_case_hasha_t *hasha);

void xt_case_hasha_iterate_remove(xt_case_hasha_t *hasha);

void xt_case_hasha_iterate_start(xt_case_hasha_t *hasha);

void *xt_case_hasha_iterate_next(xt_case_hasha_t *hasha);

xt_core_bool_t xt_case_hasha_remove(xt_case_hasha_t *hasha,
    void *decoy_object);

#endif
