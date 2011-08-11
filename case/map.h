#ifndef xt_case_map_h
#define xt_case_map_h

#include "xt/core/bool.h"
#include "xt/core/object.h"
#include "xt/core/iobject.h"

#define XT_CASE_MAP_DESTROY xt_core_bool_true
#define XT_CASE_MAP_DONT_DESTROY xt_core_bool_false

struct xt_case_map_t;
typedef struct xt_case_map_t xt_case_map_t;

xt_core_bool_t xt_case_map_add(xt_case_map_t *map, void *left,
    void *right);

int xt_case_map_compare(void *map_object_a, void *map_object_b);

void *xt_case_map_copy(void *map_object);

xt_case_map_t *xt_case_map_create(xt_core_iobject_t *left_iobject,
    xt_core_iobject_t *right_iobject, xt_core_bool_t destroys);

void xt_case_map_destroy(void *map_object);

void *xt_case_map_find(xt_case_map_t *map, void *left_object_decoy);

void xt_case_map_print(xt_case_map_t *map);

xt_core_bool_t xt_case_map_remove(xt_case_map_t *map,
    void *left_object_decoy);

#endif
