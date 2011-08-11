#include "xt/config/options.h"
#include "xt/core/tools.h"

#define DEFAULT_MODE "modex"
#define DEFAULT_OPTION "optionx"

int main(int argc, char *argv[])
{
  xt_config_options_t *options;
  char *value;

  xt_core_objects_init();

  options = xt_config_options_create(argc, argv, &xt_core_objects);
  if (!options) {
    xt_core_trace_exit("x_config_options_create");
  }

  if (xt_config_options_find_as_string(options, "mode", &value, DEFAULT_MODE)) {
    printf(":%s:\n", value);
  } else {
  }

  if (xt_config_options_find_as_string
      (options, "option", &value, DEFAULT_OPTION)) {
    printf(":%s:\n", value);
  } else {
  }

  xt_config_options_destroy(options);

  return 0;
}
