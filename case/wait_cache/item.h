#ifndef xt_case_wait_cache_item_h
#define xt_case_wait_cache_item_h

#include "xt/core/object.h"
#include "xt/core/uuid.h"

struct xt_case_wait_cache_item_t;
typedef struct xt_case_wait_cache_item_t xt_case_wait_cache_item_t;

int xt_case_wait_cache_item_compare(void *item_object_a,
    void *item_object_b);

void *xt_case_wait_cache_item_copy(void *item_object);

xt_case_wait_cache_item_t *xt_case_wait_cache_item_create
(xt_core_uuid_t *object_uuid, void *object, xt_core_object_copy_f copy,
    xt_core_object_destroy_f destroy, unsigned long lifespan_seconds);

xt_case_wait_cache_item_t *xt_case_wait_cache_item_create_decoy
(xt_core_uuid_t *object_uuid);

void xt_case_wait_cache_item_destroy(void *item_object);

void xt_case_wait_cache_item_destroy_decoy(void *item_object);

void xt_case_wait_cache_item_expire(xt_case_wait_cache_item_t *item);

void *xt_case_wait_cache_item_get_object(xt_case_wait_cache_item_t *item);

void *xt_case_wait_cache_item_get_object_copy(xt_case_wait_cache_item_t *item);

xt_core_uuid_t *xt_case_wait_cache_item_get_object_uuid
(xt_case_wait_cache_item_t *item);

xt_core_bool_t xt_case_wait_cache_item_is_expired(void *item_object);

#endif
