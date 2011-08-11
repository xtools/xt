#include "xt/core/tools.h"
#include "xt/net/star/client/conf.h"

#define DEFAULT_SERVER_EXCLUDE_IP "127.0.0.1"
#define DEFAULT_NODE_SERVER_EXCLUDE_PORT 8000
#define DEFAULT_STAR_ARM_PORT_MIN 9000
#define DEFAULT_STAR_ARM_PORT_MAX 10000

static xt_core_bool_t xt_net_star_client_conf_create_node_server_exclude_ip
(xt_net_star_client_conf_t *starclient_conf);

static xt_core_bool_t xt_net_star_client_conf_create_node_server_exclude_port
(xt_net_star_client_conf_t *starclient_conf);

static xt_core_bool_t xt_net_star_client_conf_create_star_arm_port_range
(xt_net_star_client_conf_t *starclient_conf);

static xt_core_bool_t xt_net_star_client_conf_create_star_arm_ips
(xt_net_star_client_conf_t *starclient_conf);

xt_net_star_client_conf_t *xt_net_star_client_conf_create(char *conf_filename)
{
  assert(conf_filename);
  xt_net_star_client_conf_t *starclient_conf;
  xt_core_bool_t so_far_so_good;

  starclient_conf = malloc(sizeof *starclient_conf);
  if (starclient_conf) {
    starclient_conf->conf = xt_config_file_create(conf_filename);
    if (starclient_conf->conf) {
      so_far_so_good = xt_core_bool_true;
    } else {
      so_far_so_good = xt_core_bool_false;
    }
  } else {
    xt_core_trace("malloc");
    so_far_so_good = xt_core_bool_false;
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_star_client_conf_create_node_server_exclude_ip
      (starclient_conf);
  }

  if (so_far_so_good) {
    so_far_so_good
      = xt_net_star_client_conf_create_node_server_exclude_port(starclient_conf);
  }

  if (so_far_so_good) {
    so_far_so_good
      = xt_net_star_client_conf_create_star_arm_ips(starclient_conf);
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_star_client_conf_create_star_arm_port_range
      (starclient_conf);
  }

  if (!so_far_so_good && starclient_conf) {
    if (starclient_conf->conf) {
      xt_config_file_destroy(starclient_conf->conf);
    }
    free(starclient_conf);
    starclient_conf = NULL;
  }

  return starclient_conf;
}

xt_core_bool_t xt_net_star_client_conf_create_node_server_exclude_ip
(xt_net_star_client_conf_t *starclient_conf)
{
  assert(starclient_conf);
  xt_core_bool_t success;

  if (xt_config_file_find_as_string(starclient_conf->conf,
          "node_server_exclude_ip", &starclient_conf->node_server_exclude_ip,
          DEFAULT_SERVER_EXCLUDE_IP)) {
    success = xt_core_bool_true;
  } else {
    printf("starclient conf doesn't specify node_server_exclude_ip\n");
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_net_star_client_conf_create_node_server_exclude_port
(xt_net_star_client_conf_t *starclient_conf)
{
  assert(starclient_conf);
  xt_core_bool_t success;

  if (xt_config_file_find_as_unsigned_short(starclient_conf->conf,
          "node_server_exclude_port",
          &starclient_conf->node_server_exclude_port,
          DEFAULT_NODE_SERVER_EXCLUDE_PORT)) {
    success = xt_core_bool_true;
  } else {
    printf("starclient conf doesn't specify node_server_exclude_port\n");
    starclient_conf->node_server_exclude_ip = 0;
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_net_star_client_conf_create_star_arm_ips
(xt_net_star_client_conf_t *starclient_conf)
{
  assert(starclient_conf);
  xt_core_bool_t success;

  if (xt_config_file_find_list_as_strings(starclient_conf->conf,
          "star_arm_ips[list]", &starclient_conf->star_arm_ips)) {
    success = xt_core_bool_true;
  } else {
    printf("starclient conf doesn't specify star_arm_ip[list]\n");
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_net_star_client_conf_create_star_arm_port_range
(xt_net_star_client_conf_t *starclient_conf)
{
  assert(starclient_conf);
  xt_core_bool_t success;
  xt_core_bool_t found_star_arm_port_min;
  xt_core_bool_t found_star_arm_port_max;

  found_star_arm_port_min = xt_config_file_find_as_unsigned_short
    (starclient_conf->conf, "star_arm_port_min",
        &starclient_conf->star_arm_port_min, DEFAULT_STAR_ARM_PORT_MIN);
  found_star_arm_port_max = xt_config_file_find_as_unsigned_short
    (starclient_conf->conf, "star_arm_port_max",
        &starclient_conf->star_arm_port_max, DEFAULT_STAR_ARM_PORT_MAX);

  if (found_star_arm_port_min && found_star_arm_port_max) {
    success = xt_core_bool_true;
  } else {
    printf("starclient conf doesn't specify both star_arm_port_min and "
        "star_arm_port_max\n");
    success = xt_core_bool_false;
  }

  return success;
}

void xt_net_star_client_conf_destroy(xt_net_star_client_conf_t *starclient_conf)
{
  assert(starclient_conf);
  xt_config_file_destroy(starclient_conf->conf);
  free(starclient_conf);
}
