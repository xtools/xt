#include "xt/net/http/version.h"

const char *xt_net_http_version_get_name(xt_net_http_version_t http_version)
{
  const char *name;

  switch (http_version) {

    case XT_NET_HTTP_VERSION_1_0:
      name = "HTTP/1.0";
      break;

    case XT_NET_HTTP_VERSION_1_1:
      name = "HTTP/1.1";
      break;

    case XT_NET_HTTP_VERSION_UNKNOWN:
    default:
      name = "";
      break;
  }

  return name;
}
