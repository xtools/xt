#include "xt/core/tools.h"
#include "xt/file/basic.h"
#include "xt/file/tools.h"

xt_core_bool_t xt_file_count_lines_in_file(char *filename,
    unsigned long *line_count)
{
  assert(filename);
  assert(line_count);
  xt_core_bool_t success;
  xt_file_basic_t *file;

  file = xt_file_basic_create(filename, XT_FILE_MODE_OPEN_FOR_READ);
  if (file) {
    success = xt_file_basic_get_line_count(file, line_count);
    xt_file_basic_destroy(file);
  } else {
    success = xt_core_bool_false;
    xt_core_trace("x_file_basic_create");
  }

  return success;
}
