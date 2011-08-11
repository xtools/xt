#include "xt/core/log.h"
#include "xt/file/basic.h"

int main(int argc, char *argv[])
{
  xt_core_log_t *log;
  xt_file_basic_t *disk_file_a;
  xt_file_basic_t *disk_file_b;
  FILE *disk_file_a_fp;
  FILE *disk_file_b_fp;

  disk_file_a = xt_file_basic_create("core/a.log",
      XT_FILE_MODE_TRUNCATE_OR_CREATE_FOR_WRITE);
  if (!disk_file_a) {
    xt_core_trace_exit("x_file_basic_create");
  }
  disk_file_a_fp = xt_file_basic_get_file(disk_file_a);

  log = xt_core_log_create(disk_file_a_fp);
  if (!log) {
    xt_core_trace_exit("x_audit_log_create");
  }

  xt_core_log_enter(log, "haud", "::int=%d::double=%f::string=%s::", 11111,
      2.2222, "three");

  disk_file_b = xt_file_basic_create("core/b.log",
      XT_FILE_MODE_TRUNCATE_OR_CREATE_FOR_WRITE);
  if (!disk_file_b) {
    xt_core_trace_exit("x_audit_file_create");
  }
  disk_file_b_fp = xt_file_basic_get_file(disk_file_b);

  if (!xt_core_log_add_file(log, disk_file_b_fp)) {
    xt_core_trace_exit("x_audit_log_add_file");
  }

  xt_core_log_enter(log, "haud", "this should be in both %s.log and %s.log",
      "a", "b");

  if (!xt_core_log_add_file(log, stdout)) {
    xt_core_trace_exit("x_audit_log_add_file");
  }

  xt_core_log_enter(log, "haud", "word");

  xt_core_log_enter(log, "haud", "The quick brown fox jumps over the lazy dog."
      "  The quick brown fox jumps over the lazy dog.");

  xt_core_log_destroy(log);
  xt_file_basic_destroy(disk_file_a);
  xt_file_basic_destroy(disk_file_b);

  return 0;
}
