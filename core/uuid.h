#ifndef xt_core_uuid_h
#define xt_core_uuid_h

#include "xt/core/bool.h"
#include "xt/core/message.h"
#include "xt/core/object.h"
#include "xt/core/iobject.h"

struct xt_core_uuid_t;
typedef struct xt_core_uuid_t xt_core_uuid_t;

xt_core_bool_t xt_core_uuid_add_to_message(void *uuid_object,
    xt_core_message_t *message);

int xt_core_uuid_compare(void *uuid_a_object, void *uuid_b_object);

xt_core_bool_t xt_core_uuid_compare_equal(void *uuid_a_object,
    void *uuid_b_object);

void *xt_core_uuid_copy(void *uuid_object);

xt_core_uuid_t *xt_core_uuid_create();

void *xt_core_uuid_create_from_message(xt_core_message_t *message);

xt_core_uuid_t *xt_core_uuid_create_from_string(const char *uuid_string);

xt_core_uuid_t *xt_core_uuid_create_null();

void xt_core_uuid_destroy(void *uuid_object);

char *xt_core_uuid_get_as_string(void *uuid_object);

unsigned long xt_core_uuid_get_memory_size_bytes(xt_core_uuid_t *uuid);

char *xt_core_uuid_get_string(void *uuid_object);

unsigned long xt_core_uuid_hash(xt_core_uuid_t *uuid);

void xt_core_uuid_init_iobject(xt_core_iobject_t *iobject);

xt_core_bool_t xt_core_uuid_is_null(xt_core_uuid_t *uuid);

unsigned long xt_core_uuid_mod(void *uuid_object, unsigned long divisor);

#endif
