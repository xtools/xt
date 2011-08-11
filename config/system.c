#include "xt/config/environment.h"
#include "xt/config/file.h"
#include "xt/config/options.h"
#include "xt/config/system.h"
#include "xt/core/tools.h"

struct xt_config_system_t {
  xt_config_file_t *file;
  xt_config_environment_t *environment;
  xt_config_options_t *options;
};

xt_config_system_t *xt_config_system_create(int argc, char *argv[],
    char *filename, xt_core_objects_t *objects)
{
  assert(argv);
  assert(objects);
  xt_config_system_t *system;
  xt_core_bool_t so_far_so_good;

  system = malloc(sizeof *system);
  if (system) {
    system->environment = NULL;
    system->options = NULL;
    if (filename) {
      system->file = xt_config_file_create(filename);
      if (system->file) {
        so_far_so_good = xt_core_bool_true;
      } else {
        so_far_so_good = xt_core_bool_false;
        xt_core_trace("x_config_file_create");
      }
    } else {
      so_far_so_good = xt_core_bool_true;
      system->file = NULL;
    }
  } else {
    so_far_so_good = xt_core_bool_false;
    xt_core_trace("malloc");
  }

  if (so_far_so_good) {
    system->environment = xt_config_environment_create();
    if (!system->environment) {
      so_far_so_good = xt_core_bool_false;
      xt_core_trace("x_config_environment_create");
    }
  }

  if (so_far_so_good) {
    system->options = xt_config_options_create(argc, argv, objects);
    if (!system->options) {
      so_far_so_good = xt_core_bool_false;
      xt_core_trace("x_config_options_create");
    }
  }

  if (!so_far_so_good && system) {
    if (system->file) {
      xt_config_file_destroy(system->file);
    }
    if (system->environment) {
      xt_config_environment_destroy(system->environment);
    }
    if (system->options) {
      xt_config_options_destroy(system->options);
    }
    free(system);
    system = NULL;
  }

  return system;
}

void xt_config_system_destroy(xt_config_system_t *system)
{
  assert(system);

  if (system->file) {
    xt_config_file_destroy(system->file);
  }
  xt_config_environment_destroy(system->environment);
  xt_config_options_destroy(system->options);
  free(system);
}

xt_core_bool_t xt_config_system_find(xt_config_system_t *system, char *name)
{
  assert(system);
  assert(name);
  xt_core_bool_t found_it;

  found_it = xt_core_bool_false;

  if (xt_config_options_find(system->options, name)) {
    found_it = xt_core_bool_true;
  } else {
    if (xt_config_environment_find(system->environment, name)) {
      found_it = xt_core_bool_true;
    } else {
      if (system->file && xt_config_file_find(system->file, name)) {
        found_it = xt_core_bool_true;
      }
    }
  }

  return found_it;
}

xt_core_bool_t xt_config_system_find_as_double(xt_config_system_t *system,
    char *name, double *value, double default_value)
{
  assert(system);
  assert(name);
  assert(value);
  xt_core_bool_t found_it;

  found_it = xt_core_bool_false;

  if (xt_config_options_find_as_double
      (system->options, name, value, default_value)) {
    found_it = xt_core_bool_true;
  } else {
    if (xt_config_environment_find_as_double
        (system->environment, name, value, default_value)) {
      found_it = xt_core_bool_true;
    } else {
      if (system->file && xt_config_file_find_as_double
          (system->file, name, value, default_value)) {
        found_it = xt_core_bool_true;
      }
    }
  }

  return found_it;
}

xt_core_bool_t xt_config_system_find_as_string(xt_config_system_t *system,
    char *name, char **value, char *default_value)
{
  assert(system);
  assert(name);
  assert(value);
  assert(default_value);
  xt_core_bool_t found_it;

  found_it = xt_core_bool_false;

  if (xt_config_options_find_as_string
      (system->options, name, value, default_value)) {
    found_it = xt_core_bool_true;
  } else {
    if (xt_config_environment_find_as_string
        (system->environment, name, value, default_value)) {
      found_it = xt_core_bool_true;
    } else {
      if (system->file && xt_config_file_find_as_string
          (system->file, name, value, default_value)) {
        found_it = xt_core_bool_true;
      }
    }
  }

  return found_it;
}

xt_core_bool_t xt_config_system_find_as_unsigned_long(xt_config_system_t *system,
    char *name, unsigned long *value, unsigned long default_value)
{
  assert(system);
  assert(name);
  assert(value);
  xt_core_bool_t found_it;

  found_it = xt_core_bool_false;

  if (xt_config_options_find_as_unsigned_long
      (system->options, name, value, default_value)) {
    found_it = xt_core_bool_true;
  } else {
    if (xt_config_environment_find_as_unsigned_long
        (system->environment, name, value, default_value)) {
      found_it = xt_core_bool_true;
    } else {
      if (system->file && xt_config_file_find_as_unsigned_long
          (system->file, name, value, default_value)) {
        found_it = xt_core_bool_true;
      }
    }
  }

  return found_it;
}

xt_core_bool_t xt_config_system_find_as_unsigned_short(xt_config_system_t *system,
    char *name, unsigned short *value, unsigned short default_value)
{
  assert(system);
  assert(name);
  assert(value);
  xt_core_bool_t found_it;

  found_it = xt_core_bool_false;

  if (xt_config_options_find_as_unsigned_short
      (system->options, name, value, default_value)) {
    found_it = xt_core_bool_true;
  } else {
    if (xt_config_environment_find_as_unsigned_short
        (system->environment, name, value, default_value)) {
      found_it = xt_core_bool_true;
    } else {
      if (system->file && xt_config_file_find_as_unsigned_short
          (system->file, name, value, default_value)) {
        found_it = xt_core_bool_true;
      }
    }
  }

  return found_it;
}

xt_core_bool_t xt_config_system_find_list_as_strings(xt_config_system_t *system,
    char *name, xt_case_list_t **list)
{
  assert(system);
  assert(name);
  xt_core_bool_t found_it;

  found_it = xt_core_bool_false;

  if (xt_config_options_find_list_as_strings(system->options, name, list)) {
    found_it = xt_core_bool_true;
  } else {
    if (xt_config_environment_find_list_as_strings
        (system->environment, name, list)) {
      found_it = xt_core_bool_true;
    } else {
      if (system->file
          && xt_config_file_find_list_as_strings(system->file, name, list)) {
        found_it = xt_core_bool_true;
      }
    }
  }

  return found_it;
}
