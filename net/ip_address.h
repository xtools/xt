#ifndef xt_net_ip_address_h
#define xt_net_ip_address_h

struct xt_net_ip_address_t;
typedef struct xt_net_ip_address_t xt_net_ip_address_t;

xt_net_ip_address_t *xt_net_ip_address_create_from_string(char *ip_address);

void xt_net_ip_address_destroy();

#endif
