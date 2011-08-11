#ifndef xt_net_star_node_conf_h
#define xt_net_star_node_conf_h

#include "xt/config/file.h"

struct xt_net_star_node_conf_t {
  xt_config_file_t *conf;

  char *node_ip;
  unsigned short node_port;
  unsigned short node_max_threads;

  xt_case_list_t *peer_node_ips;
  unsigned short peer_node_port_min;
  unsigned short peer_node_port_max;
};
typedef struct xt_net_star_node_conf_t xt_net_star_node_conf_t;

xt_net_star_node_conf_t *xt_net_star_node_conf_create(char *conf_filename);

void xt_net_star_node_conf_destroy(xt_net_star_node_conf_t *node_conf);

#endif
