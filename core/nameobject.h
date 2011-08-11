#ifndef xt_core_nameobject_h
#define xt_core_nameobject_h

#include "xt/core/object.h"
#include "xt/core/iobject.h"

struct xt_core_nameobject_t;
typedef struct xt_core_nameobject_t xt_core_nameobject_t;

int xt_core_nameobject_compare(void *nameobject_object_a,
    void *nameobject_object_b);

xt_core_bool_t xt_core_nameobject_compare_equal(void *nameobject_a_object,
    void *nameobject_b_object);

void *xt_core_nameobject_copy(void *nameobject_object);

xt_core_nameobject_t *xt_core_nameobject_create(char *name, void *object,
    xt_core_object_copy_f copy, xt_core_object_destroy_f destroy,
    xt_core_object_get_as_string_f get_as_string);

xt_core_nameobject_t *xt_core_nameobject_create_decoy(char *name);

void xt_core_nameobject_destroy(void *nameobject_object);

void xt_core_nameobject_destroy_decoy(void *nameobject_object);

char *xt_core_nameobject_get_as_string(void *nameobject_object);

char *xt_core_nameobject_get_name(xt_core_nameobject_t *nameobject);

void *xt_core_nameobject_get_object(xt_core_nameobject_t *nameobject);

void xt_core_nameobject_init_iobject(xt_core_iobject_t *iobject);

unsigned long xt_core_nameobject_mod(void *nameobject_object,
    unsigned long divisor);

void xt_core_nameobject_print(void *nameobject_object);

#endif
