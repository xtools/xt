#include "xt/case/list.h"
#include "xt/case/set.h"
#include "xt/core/content.h"
#include "xt/core/imessage.h"
#include "xt/core/nameobject.h"
#include "xt/core/string.h"
#include "xt/core/tools.h"
#include "xt/net/http/message.h"
#include "xt/net/http/tools.h"

struct xt_net_http_message_t {
  int client_socket;

  xt_net_http_method_t http_method;
  xt_net_http_status_t http_status;
  char *resource_path;
  xt_net_http_version_t http_version;
  xt_case_set_t *http_headers;
  char *body;
  unsigned long body_size;

  xt_core_content_t content_type;
  char *resource_name;
  xt_case_set_t *uri_parameters;

  xt_core_iobject_t nameobject_iobject;
  xt_core_iobject_t uuid_iobject;
};

static xt_core_bool_t xt_net_http_message_create_uri
(xt_net_http_message_t *http_message);

static void xt_net_http_message_create_rollback
(xt_net_http_message_t *http_message);

/*
  TODO: simplify
*/
xt_net_http_message_t *xt_net_http_message_create(int client_socket,
    xt_net_http_method_t http_method, xt_net_http_status_t http_status,
    char *resource_path, xt_net_http_version_t http_version,
    xt_case_set_t *http_headers)
{
  xt_net_http_message_t *http_message;
  xt_core_bool_t so_far_so_good;

  http_message = malloc(sizeof *http_message);
  if (http_message) {
    http_message->client_socket = client_socket;
    http_message->http_method = http_method;
    http_message->http_status = http_status;
    http_message->http_version = http_version;
    http_message->body = NULL;
    http_message->body_size = 0;
    http_message->resource_name = NULL;
    http_message->uri_parameters = NULL;
    xt_core_uuid_init_iobject(&http_message->uuid_iobject);
    xt_core_nameobject_init_iobject(&http_message->nameobject_iobject);

    if (resource_path) {
      http_message->resource_path = strdup(resource_path);
      if (http_message->resource_path) {
        so_far_so_good = xt_core_bool_true;
      } else {
        xt_core_trace("strdup");
        so_far_so_good = xt_core_bool_false;
      }
    } else {
      http_message->resource_path = NULL;
      so_far_so_good = xt_core_bool_true;
    }

  } else {
    so_far_so_good = xt_core_bool_false;
    xt_core_trace("malloc");
  }

  if (so_far_so_good) {
    if (http_headers) {
      http_message->http_headers = xt_case_set_copy(http_headers);
    } else {
      xt_core_nameobject_init_iobject(&http_message->nameobject_iobject);
      http_message->http_headers
        = xt_case_set_create(&http_message->nameobject_iobject);
      if (!http_message->http_headers) {
        so_far_so_good = xt_core_bool_false;
        xt_core_trace("x_case_set_create");
      }
    }
  }

  if (so_far_so_good) {
    if (http_message->resource_path) {
      if (!xt_net_http_message_create_uri(http_message)) {
        so_far_so_good = xt_core_bool_false;
      }
    }
  }

  if (!so_far_so_good && http_message) {
    xt_net_http_message_create_rollback(http_message);
    http_message = NULL;
  }

  assert(!http_message || http_message->http_headers);
  return http_message;
}

xt_core_bool_t xt_net_http_message_create_uri
(xt_net_http_message_t *http_message)
{
  assert(http_message->resource_path);
  xt_core_bool_t success;
  char *parameters;
  char *parameter;
  char *name;
  char *value;
  char *parameter_context;
  char *nameobject_context;
  xt_core_nameobject_t *nameobject;
  char *resource_name;
  char *decoded_value;

  nameobject_context = NULL;

  resource_name = strtok(http_message->resource_path, "?");
  http_message->resource_name = strdup(resource_name);
  /*
    TODO: check the result of the strdup!
  */
  parameters = strtok(NULL, "?");
  if (parameters) {
    http_message->uri_parameters
      = xt_case_set_create(&http_message->nameobject_iobject);
    if (http_message->uri_parameters) {
      success = xt_core_bool_true;
      parameter = strtok_r(parameters, "&", &parameter_context);
      while (parameter) {
        name = strtok_r(parameter, "=", &nameobject_context);
        value = strtok_r(NULL, "=", &nameobject_context);
        if (name && value) {
          decoded_value = xt_net_http_decode_uri(value, strlen(value));
          if (decoded_value) {
            nameobject = xt_core_nameobject_create(name, decoded_value,
                xt_core_string_copy, xt_core_string_destroy,
                xt_core_string_get_as_string);
            if (nameobject) {
              if (!xt_case_set_add
                  (http_message->uri_parameters, nameobject)) {
                xt_core_nameobject_destroy(nameobject);
              }
            } else {
              xt_core_trace("x_core_nameobject_create");
              success = xt_core_bool_false;
            }
            xt_core_string_destroy(decoded_value);
          } else {
            xt_core_trace("xt_net_http_decode_uri");
          }
        }
        parameter = strtok_r(NULL, "&", &parameter_context);
      }
    } else {
      xt_core_trace("x_case_set_create");
      success = xt_core_bool_false;
    }
  } else {
    http_message->uri_parameters = NULL;
    success = xt_core_bool_true;
  }

  return success;
}

void xt_net_http_message_create_rollback(xt_net_http_message_t *http_message)
{
  assert(http_message);

  if (http_message->resource_path) {
    free(http_message->resource_path);
  }
  if (http_message->http_headers) {
    xt_case_set_destroy(http_message->http_headers);
  }
  if (http_message->body) {
    free(http_message->body);
  }
  free(http_message);
}

void xt_net_http_message_destroy(void *http_message_object)
{
  assert(http_message_object);
  xt_net_http_message_t *http_message;

  http_message = http_message_object;

  if (http_message->body) {
    free(http_message->body);
  }

  if (http_message->resource_path) {
    free(http_message->resource_path);
  }

  if (http_message->resource_name) {
    free(http_message->resource_name);
  }

  if (http_message->uri_parameters) {
    xt_case_set_destroy(http_message->uri_parameters);
  }

  xt_case_set_destroy(http_message->http_headers);

  free(http_message);
}

char *xt_net_http_message_get_body(xt_net_http_message_t *http_message)
{
  return http_message->body;
}

unsigned long xt_net_http_message_get_body_size
(xt_net_http_message_t *http_message)
{
  return http_message->body_size;
}

int xt_net_http_message_get_client_socket(void *http_message_object)
{
  assert(http_message_object);
  xt_net_http_message_t *http_message;

  http_message = http_message_object;

  return http_message->client_socket;
}

xt_net_engine_id_t xt_net_http_message_get_engine_id(void *message_object)
{
  return XT_NET_ENGINE_HTTP;
}

xt_core_nameobject_t *xt_net_http_message_get_http_header
(xt_net_http_message_t *http_message, char *header_name)
{
  assert(http_message);
  assert(header_name);
  xt_core_nameobject_t *decoy_header;
  xt_core_nameobject_t *found_header;

  decoy_header = xt_core_nameobject_create_decoy(header_name);
  found_header = xt_case_set_find
    (http_message->http_headers, decoy_header);

  return found_header;
}

xt_case_set_t *xt_net_http_message_get_http_headers
(xt_net_http_message_t *http_message)
{
  return http_message->http_headers;
}

xt_net_http_method_t xt_net_http_message_get_http_method
(xt_net_http_message_t *http_message)
{
  return http_message->http_method;
}

xt_net_http_status_t xt_net_http_message_get_http_status
(xt_net_http_message_t *http_message)
{
  return http_message->http_status;
}

xt_net_http_version_t xt_net_http_message_get_http_version
(xt_net_http_message_t *http_message)
{
  return http_message->http_version;
}

char *xt_net_http_message_get_resource_name
(xt_net_http_message_t *http_message)
{
  return http_message->resource_name;
}

char *xt_net_http_message_get_uri_parameter
(xt_net_http_message_t *http_message, char *parameter_name)
{
  char *parameter_value;
  xt_core_nameobject_t *nameobject_decoy;
  xt_core_nameobject_t *nameobject;

  if (http_message->uri_parameters) {
    nameobject_decoy = xt_core_nameobject_create_decoy(parameter_name);
    if (nameobject_decoy) {
      nameobject
        = xt_case_set_find(http_message->uri_parameters, nameobject_decoy);
      if (nameobject) {
        parameter_value = xt_core_nameobject_get_object(nameobject);
      } else {
        parameter_value = NULL;
      }
      xt_core_nameobject_destroy_decoy(nameobject_decoy);
    } else {
      xt_core_trace("x_core_nameobject_create_decoy");
      parameter_value = NULL;
    }
  } else {
    parameter_value = NULL;
  }

  return parameter_value;
}

double xt_net_http_message_get_uri_parameter_as_double
(xt_net_http_message_t *http_message, char *parameter_name)
{
  assert(http_message);
  assert(parameter_name);
  char *value_string;
  double value_double;

  value_double = 0.0;

  value_string
    = xt_net_http_message_get_uri_parameter(http_message, parameter_name);
  if (value_string) {
    value_double = atof(value_string);
    free(value_string);
  } else {
    xt_core_trace("x_net_http_message_get_uri_parameter");
  }

  return value_double;
}

xt_core_bool_t xt_net_http_message_get_uri_parameter_as_unsigned_long
(xt_net_http_message_t *http_message, char *parameter_name,
    unsigned long *value)
{
  assert(http_message);
  assert(parameter_name);
  assert(value);
  char *value_string;
  xt_core_bool_t success;

  value_string
    = xt_net_http_message_get_uri_parameter(http_message, parameter_name);
  if (value_string) {
    success = xt_core_bool_true;
    *value = atol(value_string);
  } else {
    success = xt_core_bool_false;
    xt_core_trace("x_net_http_message_get_uri_parameter");
  }

  return success;
}

xt_core_uuid_t *xt_net_http_message_get_uri_parameter_as_uuid
(xt_net_http_message_t *http_message, char *parameter_name)
{
  assert(http_message);
  assert(parameter_name);
  xt_core_uuid_t *uuid;
  char *uuid_string;

  uuid = NULL;

  uuid_string
    = xt_net_http_message_get_uri_parameter(http_message, parameter_name);
  if (uuid_string) {
    uuid = xt_core_uuid_create_from_string(uuid_string);
    if (!uuid) {
      xt_core_trace("x_core_uuid_create_from_string");
    }
  } else {
    xt_core_trace("x_net_http_message_get_uri_parameter");
  }

  return uuid;
}

xt_case_set_t *xt_net_http_message_get_uri_parameter_as_uuid_set
(xt_net_http_message_t *http_message, char *parameter_name)
{
  assert(http_message);
  assert(parameter_name);
  xt_core_uuid_t *uuid;
  char *uuid_string;
  char *parameter_string;
  xt_case_list_t *uuid_strings;
  xt_case_set_t *uuid_set;

  uuid_set = xt_case_set_create(&http_message->uuid_iobject);
  if (uuid_set) {
    parameter_string
      = xt_net_http_message_get_uri_parameter(http_message, parameter_name);
    if (parameter_string) {
      uuid_strings
        = xt_case_list_create_strings_from_string(parameter_string, ",");
      if (uuid_strings) {
        xt_case_list_iterate_start(uuid_strings);
        while ((uuid_string = xt_case_list_iterate_next(uuid_strings))) {
          uuid = xt_core_uuid_create_from_string(uuid_string);
          if (uuid) {
            if (!xt_case_set_find(uuid_set, uuid)) {
              if (!xt_case_set_add(uuid_set, uuid)) {
                xt_core_uuid_destroy(uuid);
                xt_core_trace("x_case_set_add");
              }
            } else {
              xt_core_uuid_destroy(uuid);
            }
          } else {
            xt_core_trace("x_core_uuid_create_from_string");
          }
        }
        xt_case_list_destroy(uuid_strings);
      } else {
        xt_core_trace("x_case_list_create_string_from_strings");
      }
    } else {
      xt_core_trace("x_net_http_message_get_uri_parameter");
    }
  } else {
    xt_core_trace("x_case_set_create");
  }

  return uuid_set;
}

xt_case_set_t *xt_net_http_message_get_uri_parameters
(xt_net_http_message_t *http_message)
{
  return http_message->uri_parameters;
}

char *xt_net_http_message_get_resource_path(xt_net_http_message_t *http_message)
{
  return http_message->resource_path;
}

unsigned long xt_net_http_message_get_type(void *message_object)
{
  return XT_CORE_IMESSAGE_NO_TYPE;
}

xt_core_bool_t xt_net_http_message_set_body(xt_net_http_message_t *http_message,
    xt_core_content_t content_type, char *body, unsigned long body_size)
{
  assert(http_message);
  assert(body);
  assert(body_size > 0);
  xt_core_bool_t success;
  char *content_type_name;
  char *body_size_string;

  content_type_name = xt_core_content_get_name(content_type);
  success = xt_core_bool_true;

  if (!xt_net_http_message_set_http_header
      (http_message, "Content-Type", content_type_name)) {
    success = xt_core_bool_false;
  }

  if (asprintf(&body_size_string, "%lu", body_size) != -1) {
    if (!xt_net_http_message_set_http_header
        (http_message, "Content-Length", body_size_string)) {
      success = xt_core_bool_false;
    }
    free(body_size_string);
  } else {
    xt_core_trace("asprintf");
  }

  if (success) {
    http_message->body = malloc(body_size);
    if (http_message->body) {
      memcpy(http_message->body, body, body_size);
      http_message->body_size = body_size;
    } else {
      xt_core_trace("malloc");
      success = xt_core_bool_false;
    }
  }

  return success;
}

xt_core_bool_t xt_net_http_message_set_http_header
(xt_net_http_message_t *http_message, char *header_name, char *header_value)
{
  assert(http_message);
  assert(header_name);
  assert(header_value);
  xt_core_bool_t success;
  xt_core_nameobject_t *nameobject;

  nameobject = xt_core_nameobject_create(header_name, header_value,
      xt_core_string_copy, xt_core_string_destroy, xt_core_string_get_as_string);
  if (nameobject) {
    if (xt_case_set_add(http_message->http_headers, nameobject)) {
      success = xt_core_bool_true;
    } else {
      success = xt_core_bool_false;
    }
  } else {
    success = xt_core_bool_false;
  }

  return success;
}
