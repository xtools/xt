#include "xt/config/tools.h"
#include "xt/core/tools.h"
#include "xt/net/star/node/system.h"
#include "xt/net/star/node/conf.h"

int main(int argc, char *argv[])
{
  xt_config_disable_if_running_batch_tests(argc, argv);

  xt_net_star_node_system_t *node;
  xt_net_star_node_conf_t *node_conf;
  xt_core_log_t *log;

  node_conf = xt_net_star_node_conf_create("x_net_node.conf");
  if (!node_conf) {
    xt_core_trace_exit("x_net_node_conf_create");
  }

  log = xt_core_log_create(stdout);
  if (!log) {
    xt_core_trace_exit("x_audit_log_create");
  }

  node = xt_net_star_node_system_create(node_conf->node_ip, node_conf->node_port,
      node_conf->node_port, node_conf->node_max_threads,
      node_conf->peer_node_ips, node_conf->peer_node_port_min,
      node_conf->peer_node_port_max, log);
  if (!node) {
    xt_core_trace_exit("x_net_node_create");
  }

  xt_net_star_node_system_set_server_unresponsive_client_time_seconds(node, 32);

  if (!xt_net_star_node_system_start(node)) {
    xt_core_trace_exit("x_net_node_start");
  }

  xt_net_star_node_system_destroy(node);
  xt_net_star_node_conf_destroy(node_conf);
  xt_core_log_destroy(log);

  return 0;
}
