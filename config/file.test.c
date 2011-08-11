#include "xt/config/file.h"
#include "xt/core/tools.h"

#define DEFAULT_PORT_MIN 8000
#define DEFAULT_PORT_MAX 9000

int main(int argc, char *argv[])
{
  xt_config_file_t *conf;
  unsigned short port_min;
  unsigned short port_max;
  xt_case_list_t *node_ip_list;
  char *node_ip;

  conf = xt_config_file_create("config/file.test.conf");
  if (xt_core_bool_true) {

    if (!xt_config_file_find_as_unsigned_short
        (conf, "port_min", &port_min, DEFAULT_PORT_MIN)) {
      xt_core_trace_exit("x_config_file_find_as_unsigned_short");
    }

    if (!xt_config_file_find_as_unsigned_short
        (conf, "port_max", &port_max, DEFAULT_PORT_MAX)) {
      xt_core_trace_exit("x_config_file_find_as_unsigned_short");
    }

    if (!xt_config_file_find_list_as_strings
        (conf, "node_ips[list]", &node_ip_list)) {
      xt_core_trace_exit("x_config_file_find_list");
    }

    printf("port_min:%i\n", port_min);
    printf("port_max:%i\n", port_max);
    if (node_ip_list) {
      xt_case_list_iterate_start(node_ip_list);
      while ((node_ip = xt_case_list_iterate_next(node_ip_list))) {
        printf("node_ip:%s\n", node_ip);
      }
    }
  }
  xt_config_file_destroy(conf);

  return 0;
}
