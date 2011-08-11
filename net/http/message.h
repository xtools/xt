#ifndef xt_net_http_message_h
#define xt_net_http_message_h

#include "xt/case/set.h"
#include "xt/core/content.h"
#include "xt/core/engine.h"
#include "xt/core/nameobject.h"
#include "xt/core/object.h"
#include "xt/core/uuid.h"
#include "xt/net/http/version.h"

#define XT_NET_HTTP_MESSAGE_NO_HEADERS NULL
#define XT_NET_HTTP_MESSAGE_NO_RESOURCE_PATH NULL

enum xt_net_http_method_t {
  XT_NET_HTTP_METHOD_UNKNOWN,
  XT_NET_HTTP_METHOD_GET,
  XT_NET_HTTP_METHOD_HEAD,
  XT_NET_HTTP_METHOD_POST,
};
typedef enum xt_net_http_method_t xt_net_http_method_t;

enum xt_net_http_status_t {
  XT_NET_HTTP_STATUS_UNKNOWN,
  XT_NET_HTTP_STATUS_OK,
  XT_NET_HTTP_STATUS_NOT_FOUND,
};
typedef enum xt_net_http_status_t xt_net_http_status_t;

struct xt_net_http_message_t;
typedef struct xt_net_http_message_t xt_net_http_message_t;

xt_net_http_message_t *xt_net_http_message_create(int client_socket,
    xt_net_http_method_t http_method, xt_net_http_status_t http_status,
    char *resource_path, xt_net_http_version_t http_version,
    xt_case_set_t *http_headers);

void xt_net_http_message_destroy(void *http_message_object);

char *xt_net_http_message_get_body(xt_net_http_message_t *http_message);

unsigned long xt_net_http_message_get_body_size
(xt_net_http_message_t *http_message);

int xt_net_http_message_get_client_socket(void *http_message_object);

xt_net_engine_id_t xt_net_http_message_get_engine_id(void *message_object);

xt_core_nameobject_t *xt_net_http_message_get_http_header
(xt_net_http_message_t *http_message, char *header_name);

xt_case_set_t *xt_net_http_message_get_http_headers
(xt_net_http_message_t *http_message);

xt_net_http_method_t xt_net_http_message_get_http_method
(xt_net_http_message_t *http_message);

xt_net_http_status_t xt_net_http_message_get_http_status
(xt_net_http_message_t *http_message);

xt_net_http_version_t xt_net_http_message_get_http_version
(xt_net_http_message_t *http_message);

char *xt_net_http_message_get_resource_name
(xt_net_http_message_t *http_message);

char *xt_net_http_message_get_uri_parameter
(xt_net_http_message_t *http_message, char *parameter_name);

double xt_net_http_message_get_uri_parameter_as_double
(xt_net_http_message_t *http_message, char *parameter_name);

xt_core_bool_t xt_net_http_message_get_uri_parameter_as_unsigned_long
(xt_net_http_message_t *http_message, char *parameter_name,
    unsigned long *value);

xt_core_uuid_t *xt_net_http_message_get_uri_parameter_as_uuid
(xt_net_http_message_t *http_message, char *parameter_name);

xt_case_set_t *xt_net_http_message_get_uri_parameter_as_uuid_set
(xt_net_http_message_t *http_message, char *parameter_name);

xt_case_set_t *xt_net_http_message_get_uri_parameters
(xt_net_http_message_t *http_message);

char *xt_net_http_message_get_resource_path
(xt_net_http_message_t *http_message);

unsigned long xt_net_http_message_get_type(void *message_object);

xt_core_bool_t xt_net_http_message_set_body
(xt_net_http_message_t *http_message, xt_core_content_t content_type,
    char *body, unsigned long body_size);

xt_core_bool_t xt_net_http_message_set_http_header
(xt_net_http_message_t *http_message, char *header_name, char *header_value);

#endif
