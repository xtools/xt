#include "xt/config/options.h"
#include "xt/case/dumpster.h"
#include "xt/case/map.h"
#include "xt/core/string.h"
#include "xt/core/tools.h"

struct xt_config_options_t {
  xt_case_map_t *options;
  xt_case_dumpster_t *dumpster;
};

static void parse_options(xt_config_options_t *options, int argc, char *argv[]);

xt_config_options_t *xt_config_options_create(int argc, char *argv[],
    xt_core_objects_t *objects)
{
  xt_config_options_t *options;
  xt_core_bool_t so_far_so_good;

  options = malloc(sizeof *options);
  if (options) {
    options->dumpster = NULL;
    options->options = xt_case_map_create(&objects->string_iobject,
        &objects->string_iobject, XT_CASE_MAP_DONT_DESTROY);
    if (options->options) {
      so_far_so_good = xt_core_bool_true;
    } else {
      xt_core_trace("x_case_map_create");
      so_far_so_good = xt_core_bool_false;
    }
  } else {
    xt_core_trace("malloc");
    so_far_so_good = xt_core_bool_false;
  }

  if (so_far_so_good) {
    options->dumpster = xt_case_dumpster_create(&objects->string_iobject);
    if (!options->dumpster) {
      xt_core_trace("x_case_dumpster_create");
    }
  }

  if (so_far_so_good) {
    parse_options(options, argc, argv);
  }

  if (!so_far_so_good && options) {
    xt_case_map_destroy(options->options);
    if (options->dumpster) {
      xt_case_dumpster_destroy(options->dumpster);
    }
    free(options);
    options = NULL;
  }

  return options;
}

void xt_config_options_destroy(xt_config_options_t *options)
{
  assert(options);

  xt_case_map_destroy(options->options);
  xt_case_dumpster_destroy(options->dumpster);
  free(options);
}

xt_core_bool_t xt_config_options_find(xt_config_options_t *options, char *name)
{
  assert(options);
  assert(name);
  xt_core_bool_t found;

  if (xt_case_map_find(options->options, name)) {
    found = xt_core_bool_true;
  } else {
    found = xt_core_bool_false;
  }

  return found;
}

xt_core_bool_t xt_config_options_find_as_double(xt_config_options_t *options,
    char *name, double *value, double default_value)
{
  assert(options);
  assert(name);
  assert(value);
  xt_core_bool_t found;
  char *value_string;

  value_string = xt_case_map_find(options->options, name);
  if (value_string) {
    found = xt_core_bool_true;
    *value = atof(value_string);
  } else {
    found = xt_core_bool_false;
    *value = default_value;
  }

  return found;
}

xt_core_bool_t xt_config_options_find_as_string(xt_config_options_t *options,
    char *name, char **value, char *default_value)
{
  assert(options);
  assert(name);
  assert(value);
  assert(default_value);
  xt_core_bool_t found;

  *value = xt_case_map_find(options->options, name);
  if (*value) {
    found = xt_core_bool_true;
  } else {
    found = xt_core_bool_false;
    *value = default_value;
  }

  return found;
}

xt_core_bool_t xt_config_options_find_as_unsigned_short
(xt_config_options_t *options, char *name, unsigned short *value,
    unsigned short default_value)
{
  assert(options);
  assert(name);
  assert(value);
  xt_core_bool_t found;
  char *value_string;

  value_string = xt_case_map_find(options->options, name);
  if (value_string) {
    found = xt_core_bool_true;
    *value = atoi(value_string);
  } else {
    found = xt_core_bool_false;
    *value = default_value;
  }

  return found;
}

xt_core_bool_t xt_config_options_find_as_unsigned_long
(xt_config_options_t *options, char *name, unsigned long *value,
    unsigned long default_value)
{
  assert(options);
  assert(name);
  assert(value);
  xt_core_bool_t found;
  char *value_string;

  value_string = xt_case_map_find(options->options, name);
  if (value_string) {
    found = xt_core_bool_true;
    *value = atol(value_string);
  } else {
    found = xt_core_bool_false;
    *value = default_value;
  }

  return found;
}

xt_core_bool_t xt_config_options_find_list_as_strings
(xt_config_options_t *options, char *name, xt_case_list_t **list)
{
  xt_core_trace_exit("TODO: implement");
  return xt_core_bool_false;
}

void parse_options(xt_config_options_t *options, int argc, char *argv[])
{
  assert(options);
  int each_word;
  char *substring;
  char *name;
  char *value;

  for (each_word = 1; (each_word + 1) < argc; each_word += 2) {
    name = *(argv + each_word);
    substring = xt_core_string_substring(name, 0, 2);
    if (substring) {
      if (0 == strcmp("--", substring)) {
        name = strdup(name + 2);
        if (name) {
          if (!xt_case_dumpster_add(options->dumpster, name)) {
            xt_core_trace("x_case_dumpster_add");
          }
          value = strdup(*(argv + each_word + 1));
          if (!xt_case_dumpster_add(options->dumpster, value)) {
            xt_core_trace("x_case_dumpster_add");
          }
          if (!xt_case_map_add(options->options, name, value)) {
            xt_core_trace("x_case_map_add");
            free(name);
          }
        } else {
          xt_core_trace("strdup");
        }
      }
      free(substring);
    } else {
      xt_core_trace("x_core_string_substring");
    }
  }
}
