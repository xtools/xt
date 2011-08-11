#ifndef xt_net_http_version_h
#define xt_net_http_version_h

enum xt_net_http_version_t {
  XT_NET_HTTP_VERSION_UNKNOWN,
  XT_NET_HTTP_VERSION_1_0,
  XT_NET_HTTP_VERSION_1_1,
};
typedef enum xt_net_http_version_t xt_net_http_version_t;

const char *xt_net_http_version_get_name(xt_net_http_version_t http_version);

#endif
