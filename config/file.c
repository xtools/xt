#include "xt/config/file.h"
#include "xt/case/set.h"
#include "xt/core/nameobject.h"
#include "xt/core/string.h"
#include "xt/core/tools.h"
#include "xt/file/basic.h"

struct xt_config_file_t {
  /* TODO: reimplement with xt_case_map_t, like config_options? */
  xt_case_set_t *strings;
  xt_case_set_t *string_lists;
  xt_core_iobject_t nameobject_iobject;
};

static xt_core_bool_t find_as_string(xt_config_file_t *file, char *name,
    char **value);

static void xt_config_file_create_rollback(xt_config_file_t *conf);

static xt_core_bool_t line_is_a_comment(char *line);

static void parse_list_value(xt_config_file_t *file, char *name, char *value);

static void parse_string_value(xt_config_file_t *file, char *name, char *value);

static void read_file(xt_config_file_t *file, char *filename);

static xt_core_bool_t value_is_a_list(char *name);

xt_core_bool_t find_as_string(xt_config_file_t *file, char *name,
    char **value)
{
  assert(file);
  assert(name);
  assert(value);
  xt_core_nameobject_t *nameobject_decoy;
  xt_core_nameobject_t *nameobject;
  xt_core_bool_t found_it;

  *value = NULL;

  nameobject_decoy = xt_core_nameobject_create_decoy(name);
  if (nameobject_decoy) {
    nameobject = xt_case_set_find(file->strings, nameobject_decoy);
    xt_core_nameobject_destroy_decoy(nameobject_decoy);
    if (nameobject) {
      found_it = xt_core_bool_true;
      *value = xt_core_nameobject_get_object(nameobject);
    } else {
      found_it = xt_core_bool_false;
    }
  } else {
    found_it = xt_core_bool_false;
    xt_core_trace("x_core_nameobject_create_decoy");
  }

  return found_it;
}

xt_config_file_t *xt_config_file_create(char *filename)
{
  xt_config_file_t *file;
  xt_core_bool_t so_far_so_good;

  file = malloc(sizeof *file);
  if (file) {
    so_far_so_good = xt_core_bool_true;
  } else {
    so_far_so_good = xt_core_bool_false;
  }

  if (so_far_so_good) {
    xt_core_nameobject_init_iobject(&file->nameobject_iobject);
    file->strings = xt_case_set_create(&file->nameobject_iobject);
    if (!file->strings) {
      so_far_so_good = xt_core_bool_false;
    }
  }

  if (so_far_so_good) {
    file->string_lists = xt_case_set_create(&file->nameobject_iobject);
    if (!file->string_lists) {
      so_far_so_good = xt_core_bool_false;
    }
  }

  if (so_far_so_good) {
    if (xt_file_basic_exists(filename)) {
      read_file(file, filename);
    }
  }

  if (!so_far_so_good && file) {
    xt_config_file_create_rollback(file);
    file = NULL;
  }

  return file;
}

void xt_config_file_create_rollback(xt_config_file_t *file)
{
  assert(file);

  if (file->strings) {
    xt_case_set_destroy(file->strings);
  }
  if (file->string_lists) {
    xt_case_set_destroy(file->string_lists);
  }
  free(file);

}

void xt_config_file_destroy(xt_config_file_t *file)
{
  assert(file);
  assert(file->strings);
  assert(file->string_lists);

  xt_case_set_destroy(file->strings);
  xt_case_set_destroy(file->string_lists);
  free(file);

}

xt_core_bool_t xt_config_file_find(xt_config_file_t *file, char *name)
{
  assert(file);
  assert(name);
  xt_core_nameobject_t *nameobject_decoy;
  xt_core_bool_t found_it;

  nameobject_decoy = xt_core_nameobject_create_decoy(name);
  if (nameobject_decoy) {
    if (xt_case_set_find(file->strings, nameobject_decoy)) {
      found_it = xt_core_bool_true;
    } else {
      found_it = xt_core_bool_false;
    }
    xt_core_nameobject_destroy_decoy(nameobject_decoy);
  } else {
    found_it = xt_core_bool_false;
    xt_core_trace("x_core_nameobject_create_decoy");
  }

  return found_it;
}

xt_core_bool_t xt_config_file_find_as_double(xt_config_file_t *file, char *name,
    double *value, double default_value)
{
  assert(file);
  assert(name);
  assert(value);
  xt_core_bool_t found;
  char *value_string;

  if (find_as_string(file, name, &value_string)) {
    found = xt_core_bool_true;
    *value = atof(value_string);
  } else {
    found = xt_core_bool_false;
    *value = default_value;
  }

  return found;
}

xt_core_bool_t xt_config_file_find_as_string(xt_config_file_t *file, char *name,
    char **value, char *default_value)
{
  assert(file);
  assert(name);
  assert(value);
  assert(default_value);
  xt_core_bool_t found_it;

  if (find_as_string(file, name, value)) {
    found_it = xt_core_bool_true;
  } else {
    found_it = xt_core_bool_false;
    *value = default_value;
  }

  return found_it;
}

xt_core_bool_t xt_config_file_find_as_unsigned_long(xt_config_file_t *file,
    char *name, unsigned long *value, unsigned long default_value)
{
  assert(file);
  assert(name);
  assert(value);
  char *value_string;
  xt_core_bool_t found;

  if (find_as_string(file, name, &value_string)) {
    found = xt_core_bool_true;
    *value = atol(value_string);
  } else {
    found = xt_core_bool_false;
    *value = default_value;
  }

  return found;
}

xt_core_bool_t xt_config_file_find_as_unsigned_short(xt_config_file_t *file,
    char *name, unsigned short *value, unsigned short default_value)
{
  assert(file);
  assert(name);
  assert(value);
  unsigned long value_unsigned_long;
  xt_core_bool_t success;

  success = xt_config_file_find_as_unsigned_long
    (file, name, &value_unsigned_long, default_value);
  if (success) {
    *value = (unsigned short) value_unsigned_long;
  }

  return success;
}

xt_core_bool_t xt_config_file_find_list_as_strings(xt_config_file_t *file,
    char *name, xt_case_list_t **list)
{
  assert(file);
  assert(name);
  xt_core_nameobject_t *nameobject_decoy;
  xt_core_nameobject_t *nameobject;
  xt_core_bool_t success;

  *list = NULL;

  nameobject_decoy = xt_core_nameobject_create_decoy(name);
  if (nameobject_decoy) {
    nameobject = xt_case_set_find(file->string_lists, nameobject_decoy);
    xt_core_nameobject_destroy_decoy(nameobject_decoy);
    if (nameobject) {
      success = xt_core_bool_true;
      *list = xt_core_nameobject_get_object(nameobject);
    } else {
      success = xt_core_bool_false;
    }
  } else {
    success = xt_core_bool_false;
    xt_core_trace("x_core_nameobject_create_decoy");
  }

  return success;
}

xt_core_bool_t line_is_a_comment(char *line)
{
  assert(line);
  xt_core_bool_t is_a_comment;

  if (strlen(line) > 0) {
    if ('#' == *line) {
      is_a_comment = xt_core_bool_true;
    } else {
      is_a_comment = xt_core_bool_false;
    }
  } else {
    is_a_comment = xt_core_bool_true;
  }

  return is_a_comment;
}

void parse_list_value(xt_config_file_t *file, char *name, char *value)
{
  assert(file);
  assert(name);
  assert(value);
  xt_core_nameobject_t *nameobject;
  xt_case_list_t *list;
  char *string;
  char *string_copy;
  char *strtok_context;

  list = xt_case_list_create(xt_core_string_compare, xt_core_string_copy,
      xt_core_string_destroy);
  if (list) {
    string = strtok_r(value, ",", &strtok_context);
    while (string) {
      string_copy = strdup(string);
      if (string_copy) {
        if (!xt_case_list_add_last(list, string_copy)) {
          xt_core_trace("x_case_list_add_last");
        }
      } else {
        xt_core_trace("strdup");
      }
      string = strtok_r(NULL, ",", &strtok_context);
    }
    nameobject = xt_core_nameobject_create(name, list, XT_CORE_OBJECT_NO_COPY_F,
        xt_case_list_destroy, xt_case_list_get_as_string);
    if (nameobject) {
      if (!xt_case_set_add(file->string_lists, nameobject)) {
        xt_core_trace("x_case_set_add");
        xt_core_nameobject_destroy(nameobject);
      }
    } else {
      xt_core_trace("x_core_nameobject_create");
    }
  } else {
    xt_core_trace("x_case_list_create");
  }

}

void parse_string_value(xt_config_file_t *file, char *name, char *value)
{
  assert(file);
  assert(name);
  assert(value);
  xt_core_nameobject_t *nameobject;

  nameobject = xt_core_nameobject_create(name, value, xt_core_string_copy,
      xt_core_string_destroy, xt_core_string_get_as_string);
  if (nameobject) {
    if (!xt_case_set_add(file->strings, nameobject)) {
      xt_core_trace("x_case_set_add");
    }
  } else {
    xt_core_trace("x_core_nameobject_create");
  }

}

void read_file(xt_config_file_t *file, char *filename)
{
  assert(file);
  assert(filename);
  xt_file_basic_t *file_basic;
  xt_case_list_t *lines;
  char *line;
  char *name;
  char *value;
  char *strtok_context;

  file_basic = xt_file_basic_create(filename, XT_FILE_MODE_OPEN_FOR_READ);
  if (file_basic) {
    lines = xt_file_basic_get_as_line_list(file_basic);
    if (lines) {
      xt_case_list_iterate_start(lines);
      while ((line = xt_case_list_iterate_next(lines))) {
        if (!line_is_a_comment(line)) {
          name = strtok_r(line, ":", &strtok_context);
          if (name) {
            value = strtok_r(NULL, "\n", &strtok_context);
            if (!value) {
              value = "";
            }
            if (value_is_a_list(name)) {
              parse_list_value(file, name, value);
            } else {
              parse_string_value(file, name, value);
            }
          }
        }
      }
      xt_case_list_destroy(lines);
    }
    xt_file_basic_destroy(file_basic);
  } else {
    xt_core_trace("x_core_file_create");
  }
}

xt_core_bool_t value_is_a_list(char *name)
{
  assert(name);
  xt_core_bool_t is_a_list;
  unsigned short name_size;
  unsigned short start_compare_position;

  name_size = strlen(name);

  if (name_size >= 6) {
    start_compare_position = name_size - 6;
    if (0 == strcmp("[list]", name + start_compare_position)) {
      is_a_list = xt_core_bool_true;
    } else {
      is_a_list = xt_core_bool_false;
    }
  } else {
    is_a_list = xt_core_bool_false;
  }

  return is_a_list;
}
