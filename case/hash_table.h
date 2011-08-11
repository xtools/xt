#ifndef xt_case_hash_table_h
#define xt_case_hash_table_h

#include "xt/core/object.h"
#include "xt/core/iobject.h"
#include "xt/core/tools.h"

struct xt_case_hash_table_t;
typedef struct xt_case_hash_table_t xt_case_hash_table_t;

xt_core_bool_t xt_case_hash_table_add(xt_case_hash_table_t *hash_table,
    void *object);

void xt_case_hash_table_clear(xt_case_hash_table_t *hash_table);

xt_case_hash_table_t *xt_case_hash_table_create(xt_core_iobject_t *iobject);

void xt_case_hash_table_destroy(xt_case_hash_table_t *hash_table);

void xt_case_hash_table_dont_destroy_objects(xt_case_hash_table_t *hash_table);

void *xt_case_hash_table_find(xt_case_hash_table_t *hash_table,
    void *decoy_object);

xt_core_iobject_t *xt_case_hash_table_get_iobject
(xt_case_hash_table_t *hash_table);

unsigned long xt_case_hash_table_get_size(xt_case_hash_table_t *hash_table);

void xt_case_hash_table_iterate_remove(xt_case_hash_table_t *hash_table);

void xt_case_hash_table_iterate_start(xt_case_hash_table_t *hash_table);

void *xt_case_hash_table_iterate_next(xt_case_hash_table_t *hash_table);

xt_core_bool_t xt_case_hash_table_remove(xt_case_hash_table_t *hash_table,
    void *decoy_object);

#endif
