#include "xt/core/tools.h"
#include "xt/net/star/node/conf.h"

#define DEFAULT_NODE_IP "127.0.0.1"
#define DEFAULT_MAX_THREADS 2
#define DEFAULT_NODE_PORT 67
#define DEFAULT_PEER_NODE_PORT_MIN 8000
#define DEFAULT_PEER_NODE_PORT_MAX 9000

static xt_core_bool_t xt_net_star_node_conf_create_node_ip
(xt_net_star_node_conf_t *node_conf);

static xt_core_bool_t xt_net_star_node_conf_create_node_max_threads
(xt_net_star_node_conf_t *node_conf);

static xt_core_bool_t xt_net_star_node_conf_create_node_port
(xt_net_star_node_conf_t *node_conf);

static xt_core_bool_t xt_net_star_node_conf_create_peer_node_ips
(xt_net_star_node_conf_t *node_conf);

static xt_core_bool_t xt_net_star_node_conf_create_peer_node_port_range
(xt_net_star_node_conf_t *node_conf);

xt_net_star_node_conf_t *xt_net_star_node_conf_create(char *conf_filename)
{
  assert(conf_filename);
  xt_net_star_node_conf_t *node_conf;
  xt_core_bool_t so_far_so_good;

  node_conf = malloc(sizeof *node_conf);
  if (node_conf) {
    node_conf->conf = xt_config_file_create(conf_filename);
    if (node_conf->conf) {
      so_far_so_good = xt_core_bool_true;
    } else {
      so_far_so_good = xt_core_bool_false;
      xt_core_trace("new");
    }
  } else {
    xt_core_trace("malloc");
    so_far_so_good = xt_core_bool_false;
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_star_node_conf_create_node_ip(node_conf);
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_star_node_conf_create_node_max_threads(node_conf);
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_star_node_conf_create_node_port(node_conf);
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_star_node_conf_create_peer_node_ips(node_conf);
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_star_node_conf_create_peer_node_port_range(node_conf);
  }

  if (!so_far_so_good && node_conf) {
    if (node_conf->conf) {
      xt_config_file_destroy(node_conf->conf);
    }
    free(node_conf);
    node_conf = NULL;
  }

  return node_conf;
}

xt_core_bool_t xt_net_star_node_conf_create_node_ip(xt_net_star_node_conf_t *node_conf)
{
  assert(node_conf);
  xt_core_bool_t success;

  if (xt_config_file_find_as_string
      (node_conf->conf, "node-ip", &node_conf->node_ip, DEFAULT_NODE_IP)) {
    success = xt_core_bool_true;
  } else {
    printf("node conf doesn't specify node-ip\n");
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_net_star_node_conf_create_node_max_threads
(xt_net_star_node_conf_t *node_conf)
{
  assert(node_conf);
  xt_core_bool_t success;

  success = xt_config_file_find_as_unsigned_short(node_conf->conf,
      "node-max-threads", &node_conf->node_max_threads, DEFAULT_MAX_THREADS);
  if (success) {
    success = xt_core_bool_true;
  } else {
    printf("node conf doesn't specify node-max-threads\n");
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_net_star_node_conf_create_node_port(xt_net_star_node_conf_t *node_conf)
{
  assert(node_conf);
  xt_core_bool_t success;

  if (xt_config_file_find_as_unsigned_short(node_conf->conf, "node-port",
          &node_conf->node_port, DEFAULT_NODE_PORT)) {
    success = xt_core_bool_true;
  } else {
    printf("node conf doesn't specify node-port\n");
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_net_star_node_conf_create_peer_node_ips
(xt_net_star_node_conf_t *node_conf)
{
  assert(node_conf);
  xt_core_bool_t success;

  if (xt_config_file_find_list_as_strings
      (node_conf->conf, "peer-node-ips[list]", &node_conf->peer_node_ips)) {
    success = xt_core_bool_true;
  } else {
    printf("node conf doesn't specify peer-node-ip[list]\n");
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_net_star_node_conf_create_peer_node_port_range
(xt_net_star_node_conf_t *node_conf)
{
  assert(node_conf);
  xt_core_bool_t success;
  xt_core_bool_t found_peer_node_port_min;
  xt_core_bool_t found_peer_node_port_max;

  found_peer_node_port_min = xt_config_file_find_as_unsigned_short
    (node_conf->conf, "peer-node-port-min", &node_conf->peer_node_port_min,
        DEFAULT_PEER_NODE_PORT_MIN);
  found_peer_node_port_max = xt_config_file_find_as_unsigned_short
    (node_conf->conf, "peer-node-port-max", &node_conf->peer_node_port_max,
        DEFAULT_PEER_NODE_PORT_MAX);
  if (found_peer_node_port_min && found_peer_node_port_max) {
    success = xt_core_bool_true;
  } else {
    printf("node conf doesn't specify both peer-node-port-min and "
        "peer-node-port-max\n");
    success = xt_core_bool_false;
  }

  return success;
}

void xt_net_star_node_conf_destroy(xt_net_star_node_conf_t *node_conf)
{
  assert(node_conf);
  xt_config_file_destroy(node_conf->conf);
  free(node_conf);
}
