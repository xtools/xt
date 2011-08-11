#include "xt/file/directory.h"
#include "xt/core/tools.h"

struct xt_file_directory_t {};

xt_file_directory_t *xt_file_directory_create()
{
  return NULL;
}

void xt_file_directory_destroy(xt_file_directory_t *xt_file_directory)
{
}

xt_core_bool_t xt_file_directory_exists(char *directory_name)
{
  assert(directory_name);
  xt_core_bool_t exists;
  DIR *dir;

  dir = opendir(directory_name);
  if (dir) {
    exists = xt_core_bool_true;
    if (0 != closedir(dir)) {
      xt_core_trace("closedir");
    }
  } else {
    exists = xt_core_bool_false;
  }

  return exists;
}

xt_core_bool_t xt_file_directory_make(char *directory_name, mode_t mode)
{
  assert(directory_name);
  xt_core_bool_t success;

  if (0 == mkdir(directory_name, mode)) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}
