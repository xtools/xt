#ifndef xt_net_star_client_conf_h
#define xt_net_star_client_conf_h

#include "xt/config/file.h"

struct xt_net_star_client_conf_t {
  xt_config_file_t *conf;

  xt_case_list_t *star_arm_ips;
  unsigned short star_arm_port_min;
  unsigned short star_arm_port_max;

  char *node_server_exclude_ip;
  unsigned short node_server_exclude_port;
};
typedef struct xt_net_star_client_conf_t xt_net_star_client_conf_t;

xt_net_star_client_conf_t *xt_net_star_client_conf_create(char *conf_filename);

void xt_net_star_client_conf_destroy
(xt_net_star_client_conf_t *starclient_conf);

#endif
