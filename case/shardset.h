#ifndef xt_case_shardset_h
#define xt_case_shardset_h

#include "xt/core/object.h"

#define XT_CASE_SHARDSET_MAX_SHARDS 32768

struct xt_case_shardset_t;
typedef struct xt_case_shardset_t xt_case_shardset_t;

xt_core_bool_t xt_case_shardset_add(xt_case_shardset_t *shardset,
    void *object);

xt_core_bool_t xt_case_shardset_add_replace(xt_case_shardset_t *shardset,
    void *object);

void xt_case_shardset_clear(xt_case_shardset_t *shardset);

xt_case_shardset_t *xt_case_shardset_create(xt_core_object_compare_f compare,
    xt_core_object_compare_equal_f compare_equal, xt_core_object_copy_f copy,
    xt_core_object_destroy_f destroy, xt_core_object_hash_f hash_object,
    xt_core_object_mod_f mod, unsigned short shard_count);

void xt_case_shardset_destroy(xt_case_shardset_t *shardset);

void *xt_case_shardset_find(xt_case_shardset_t *shardset, void *object);

void *xt_case_shardset_find_copy(xt_case_shardset_t *shardset, void *object);

unsigned long xt_case_shardset_get_size(xt_case_shardset_t *shardset);

void *xt_case_shardset_iterate_next(xt_case_shardset_t *shardset);

void xt_case_shardset_lock(xt_case_shardset_t *shardset);

void xt_case_shardset_iterate_remove(xt_case_shardset_t *shardset);

void xt_case_shardset_iterate_start(xt_case_shardset_t *shardset);

void xt_case_shardset_print(xt_case_shardset_t *shardset,
    xt_core_object_get_as_string_f get_object_as_string);

xt_core_bool_t xt_case_shardset_remove(xt_case_shardset_t *shardset,
    void *object);

unsigned long xt_case_shardset_remove_if(xt_case_shardset_t *shardset,
    xt_core_object_evaluate_condition_f object_condition);

void xt_case_shardset_unlock(xt_case_shardset_t *shardset);

#endif
