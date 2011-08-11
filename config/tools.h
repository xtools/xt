#ifndef xt_config_tools_h
#define xt_config_tools_h

#include "xt/core/bool.h"

void xt_config_disable_if_running_batch_tests(int argc, char *argv[]);

xt_core_bool_t xt_config_running_batch_tests(int argc, char *argv[]);

#endif
