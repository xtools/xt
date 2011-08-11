#include "xt/config/tools.h"
#include "xt/config/options.h"
#include "xt/core/tools.h"

void xt_config_disable_if_running_batch_tests(int argc, char *argv[])
{
  if (xt_config_running_batch_tests(argc, argv)) {
    printf("  %s disabled in batch test mode\n", *argv);
    exit(0);
  }
}

xt_core_bool_t xt_config_running_batch_tests(int argc, char *argv[])
{
  xt_config_options_t *options;
  xt_core_bool_t batch;

  batch = xt_core_bool_false;
  xt_core_objects_init();

  options = xt_config_options_create(argc, argv, &xt_core_objects);
  if (options) {
    if (xt_config_options_find(options, "running-batch-tests")) {
      batch = xt_core_bool_true;
    }
    xt_config_options_destroy(options);
  } else {
    xt_core_trace("x_config_options_create");
  }

  return batch;
}
