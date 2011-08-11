#ifndef xt_config_options_h
#define xt_config_options_h

#include "xt/case/list.h"
#include "xt/core/objects.h"

struct xt_config_options_t;
typedef struct xt_config_options_t xt_config_options_t;

xt_config_options_t *xt_config_options_create(int argc, char *argv[],
    xt_core_objects_t *objects);

void xt_config_options_destroy(xt_config_options_t *options);

xt_core_bool_t xt_config_options_find(xt_config_options_t *options, char *name);

xt_core_bool_t xt_config_options_find_as_double(xt_config_options_t *options,
    char *name, double *value, double default_value);

xt_core_bool_t xt_config_options_find_as_string(xt_config_options_t *options,
    char *name, char **value, char *default_value);

xt_core_bool_t xt_config_options_find_as_unsigned_short
(xt_config_options_t *options, char *name, unsigned short *value,
    unsigned short default_value);

xt_core_bool_t xt_config_options_find_as_unsigned_long
(xt_config_options_t *options, char *name, unsigned long *value,
    unsigned long default_value);

xt_core_bool_t xt_config_options_find_list_as_strings
(xt_config_options_t *options, char *name, xt_case_list_t **list);

#endif
