#include "xt/core/content.h"

static char *content_names[XT_CORE_CONTENT_TYPE_COUNT] = {
  "void",
  "image/gif",
  "text/css",
  "text/html",
  "text/javascript"
};

char *xt_core_content_get_name(xt_core_content_t content_type)
{
  char *content_name;

  if (content_type < XT_CORE_CONTENT_TYPE_COUNT) {
    content_name = content_names[content_type];
  } else {
    content_name = "[unknown]";
  }

  return content_name;
}
