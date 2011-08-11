#include "xt/core/string.h"
#include "xt/core/standard.h"
#include "xt/core/tools.h"
#include "xt/net/http/tools.h"

char *encoded_uri = "%3D%29";

int main(int argc, char *argv[])
{
  char *decoded_uri;

  decoded_uri = xt_net_http_decode_uri(encoded_uri, strlen(encoded_uri));
  if (decoded_uri) {
    printf("%s\n", decoded_uri);
    xt_core_string_destroy(decoded_uri);
  } else {
    xt_core_trace("xt_net_http_decode_uri");
  }

  return 0;
}
