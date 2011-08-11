#include "xt/core/tools.h"
#include "xt/config/environment.h"

struct xt_config_environment_t {
};

xt_config_environment_t *xt_config_environment_create()
{
  xt_config_environment_t *environment;

  environment = malloc(sizeof *environment);
  if (environment) {

  } else {
    xt_core_trace("malloc");
  }

  return environment;
}

void xt_config_environment_destroy(xt_config_environment_t *environment)
{
  assert(environment);
  free(environment);
}

xt_core_bool_t xt_config_environment_find(xt_config_environment_t *environment,
    char *name)
{
  assert(environment);
  assert(name);

  return xt_core_bool_false;
}

xt_core_bool_t xt_config_environment_find_as_double
(xt_config_environment_t *environment, char *name, double *value,
    double default_value)
{
  assert(environment);
  assert(name);
  assert(value);

  return xt_core_bool_false;
}

xt_core_bool_t xt_config_environment_find_as_string
(xt_config_environment_t *environment, char *name, char **value,
    char *default_value)
{
  assert(environment);
  assert(name);
  assert(value);
  assert(default_value);

  return xt_core_bool_false;
}

xt_core_bool_t xt_config_environment_find_as_unsigned_long
(xt_config_environment_t *environment, char *name, unsigned long *value,
    unsigned long default_value)
{
  assert(environment);
  assert(name);
  assert(value);

  return xt_core_bool_false;
}

xt_core_bool_t xt_config_environment_find_as_unsigned_short
(xt_config_environment_t *environment, char *name, unsigned short *value,
    unsigned short default_value)
{
  assert(environment);
  assert(name);
  assert(value);

  return xt_core_bool_false;
}

xt_core_bool_t xt_config_environment_find_list_as_strings
(xt_config_environment_t *environment, char *name, xt_case_list_t **list)
{
  assert(environment);
  assert(name);
  assert(list);

  return xt_core_bool_false;
}
