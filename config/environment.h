#ifndef xt_config_environment_h
#define xt_config_environment_h

#include "xt/case/list.h"

struct xt_config_environment_t;
typedef struct xt_config_environment_t xt_config_environment_t;

xt_config_environment_t *xt_config_environment_create();

void xt_config_environment_destroy(xt_config_environment_t *environment);

xt_core_bool_t xt_config_environment_find(xt_config_environment_t *environment,
    char *name);

xt_core_bool_t xt_config_environment_find_as_double
(xt_config_environment_t *environment, char *name, double *value,
    double default_value);

xt_core_bool_t xt_config_environment_find_as_string
(xt_config_environment_t *environment, char *name, char **value,
    char *default_value);

xt_core_bool_t xt_config_environment_find_as_unsigned_long
(xt_config_environment_t *environment, char *name, unsigned long *value,
    unsigned long default_value);

xt_core_bool_t xt_config_environment_find_as_unsigned_short
(xt_config_environment_t *environment, char *name, unsigned short *value,
    unsigned short default_value);

xt_core_bool_t xt_config_environment_find_list_as_strings
(xt_config_environment_t *environment, char *name, xt_case_list_t **list);

#endif
