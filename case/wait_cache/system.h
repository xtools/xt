#ifndef xt_case_wait_cache_system_h
#define xt_case_wait_cache_system_h

#include "xt/core/object.h"
#include "xt/core/uuid.h"

struct xt_case_wait_cache_system_t;
typedef struct xt_case_wait_cache_system_t xt_case_wait_cache_system_t;

xt_core_bool_t xt_case_wait_cache_system_add
(xt_case_wait_cache_system_t *system, xt_core_uuid_t *object_uuid,
    void *object, xt_core_object_copy_f copy,
    xt_core_object_destroy_f destroy, unsigned long lifetime_seconds);

void xt_case_wait_cache_system_clear(xt_case_wait_cache_system_t *system);

xt_case_wait_cache_system_t *xt_case_wait_cache_system_create();

void xt_case_wait_cache_system_destroy
(xt_case_wait_cache_system_t *system);

void *xt_case_wait_cache_system_find_wait_copy_expire
(xt_case_wait_cache_system_t *system, xt_core_uuid_t *object_uuid,
    unsigned long max_wait_seconds);

unsigned long xt_case_wait_cache_system_get_size
(xt_case_wait_cache_system_t *system);

void xt_case_wait_cache_system_remove_objects
(xt_case_wait_cache_system_t *system);

#endif
