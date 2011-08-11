#ifndef xt_case_junk_drawer_h
#define xt_case_junk_drawer_h

#include "xt/core/object.h"
#include "xt/core/iobject.h"
#include "xt/core/tools.h"

struct xt_case_junk_drawer_t;
typedef struct xt_case_junk_drawer_t xt_case_junk_drawer_t;

xt_core_bool_t xt_case_junk_drawer_add
(xt_case_junk_drawer_t *junk_drawer, void *object);

void xt_case_junk_drawer_clear(xt_case_junk_drawer_t *junk_drawer);

xt_case_junk_drawer_t *xt_case_junk_drawer_create
(xt_core_iobject_t *iobject);

void xt_case_junk_drawer_destroy(xt_case_junk_drawer_t *junk_drawer);

void xt_case_junk_drawer_dont_destroy_objects
(xt_case_junk_drawer_t *junk_drawer);

void *xt_case_junk_drawer_find(xt_case_junk_drawer_t *junk_drawer,
    void *decoy_object);

xt_core_iobject_t *xt_case_junk_drawer_get_iobject
(xt_case_junk_drawer_t *junk_drawer);

unsigned long xt_case_junk_drawer_get_size
(xt_case_junk_drawer_t *junk_drawer);

void xt_case_junk_drawer_iterate_remove
(xt_case_junk_drawer_t *junk_drawer);

void xt_case_junk_drawer_iterate_start(xt_case_junk_drawer_t *junk_drawer);

void *xt_case_junk_drawer_iterate_next(xt_case_junk_drawer_t *junk_drawer);

xt_core_bool_t xt_case_junk_drawer_remove
(xt_case_junk_drawer_t *junk_drawer, void *decoy_object);

#endif
