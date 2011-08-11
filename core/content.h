#ifndef xt_core_content_h
#define xt_core_content_h

#define XT_CORE_CONTENT_TYPE_COUNT 5
enum xt_core_content_t {
  XT_CORE_CONTENT_UNKNOWN = 0,
  XT_CORE_CONTENT_MIME_IMAGE_GIF = 1,
  XT_CORE_CONTENT_MIME_TEXT_CSS = 2,
  XT_CORE_CONTENT_MIME_TEXT_HTML = 3,
  XT_CORE_CONTENT_MIME_TEXT_JAVASCRIPT = 4,
};
typedef enum xt_core_content_t xt_core_content_t;

char *xt_core_content_get_name(xt_core_content_t content_type);

#endif
