#ifndef xt_config_system_h
#define xt_config_system_h

#include "xt/core/bool.h"
#include "xt/core/objects.h"
#include "xt/case/list.h"

struct xt_config_system_t;
typedef struct xt_config_system_t xt_config_system_t;

xt_config_system_t *xt_config_system_create(int argc, char *argv[],
    char *filename, xt_core_objects_t *objects);

void xt_config_system_destroy(xt_config_system_t *system);

xt_core_bool_t xt_config_system_find(xt_config_system_t *system, char *name);

xt_core_bool_t xt_config_system_find_as_double(xt_config_system_t *system,
    char *name, double *value, double default_value);

xt_core_bool_t xt_config_system_find_as_string(xt_config_system_t *system,
    char *name, char **value, char *default_value);

xt_core_bool_t xt_config_system_find_as_unsigned_long(xt_config_system_t *system,
    char *name, unsigned long *value, unsigned long default_value);

xt_core_bool_t xt_config_system_find_as_unsigned_short(xt_config_system_t *system,
    char *name, unsigned short *value, unsigned short default_value);

xt_core_bool_t xt_config_system_find_list_as_strings(xt_config_system_t *system,
    char *name, xt_case_list_t **list);

#endif
