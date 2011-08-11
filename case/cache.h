#ifndef xt_case_cache_h
#define xt_case_cache_h

#include "xt/core/object.h"
#include "xt/sync/qutex.h"

struct xt_case_cache_t;
typedef struct xt_case_cache_t xt_case_cache_t;

xt_core_bool_t xt_case_cache_add(xt_case_cache_t *cache, void *object);

void xt_case_cache_clear(xt_case_cache_t *cache);

xt_case_cache_t *xt_case_cache_create(xt_core_object_compare_f compare,
    xt_core_object_copy_f copy, xt_core_object_destroy_f destroy,
    xt_core_object_evaluate_condition_f remove_condition);

void xt_case_cache_destroy(xt_case_cache_t *cache);

void *xt_case_cache_find(xt_case_cache_t *cache, void *object);

void *xt_case_cache_find_copy(xt_case_cache_t *cache, void *object);

unsigned long xt_case_cache_get_size(xt_case_cache_t *cache);

void xt_case_cache_remove_objects(xt_case_cache_t *cache);

#endif
