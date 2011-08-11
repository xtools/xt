#include "xt/case/list.h"
#include "xt/core/nameobject.h"
#include "xt/core/string.h"
#include "xt/core/tools.h"
#include "xt/net/http/message.h"
#include "xt/net/http/post.h"
#include "xt/net/socket.h"

#define IN_BUFFER_SIZE 4096
#define STATUS_STRING_OK "200 OK"
#define STATUS_STRING_NOT_FOUND "404 Not Found"

struct xt_net_http_post_t {
  int socket;

  xt_case_list_t *inbox;
  xt_case_list_t *outbox;

  char *out_buffer;
  unsigned long out_buffer_size;
  xt_core_bool_t currently_sending_out_buffer;
  unsigned long out_buffer_send_position;

  char in_buffer[IN_BUFFER_SIZE];
  xt_core_bool_t in_buffer_have_status_line;
  xt_core_bool_t in_buffer_have_headers;
  xt_core_bool_t in_buffer_expecting_body;
  xt_core_bool_t in_buffer_have_body;
  xt_core_bool_t in_buffer_have_complete_message;
  unsigned long in_buffer_receive_position;
  unsigned long in_buffer_parse_position;
  xt_net_http_method_t in_http_method;
  char *in_resource_path;
  xt_net_http_version_t in_http_version;
  xt_case_set_t *in_http_headers;
  char *in_body;

  time_t last_receive_activity_time;
  xt_core_bool_t socket_closed;

  xt_net_post_stats_t stats;

  xt_core_iobject_t nameobject_iobject;
};

static char *get_header_lines(xt_net_http_message_t *http_message,
    unsigned short *header_lines_size);

static char *get_outgoing_status_line(xt_net_http_message_t *http_message,
    unsigned short *status_line_size);

static const char *get_outgoing_status_line_status_string
(xt_net_http_status_t http_status, unsigned short *status_string_size);

static void xt_net_http_post_create_rollback(xt_net_http_post_t *http_post);

static xt_net_http_method_t parse_http_method(char *http_method_string);

static xt_net_http_version_t parse_http_version(char *http_version_string);

static void parse_incoming_message(xt_net_http_post_t *http_post);

static void parse_incoming_message_status_line(xt_net_http_post_t *http_post);

static void parse_incoming_message_headers(xt_net_http_post_t *http_post);

static void parse_incoming_message_body(xt_net_http_post_t *http_post);

static xt_core_bool_t put_message_into_out_buffer(xt_net_http_post_t *http_post,
    xt_net_http_message_t *http_message);

static xt_core_bool_t put_received_message_in_inbox
(xt_net_http_post_t *http_post);

static xt_core_bool_t receive_messages(xt_net_http_post_t *http_post);

static void reset_for_next_receive(xt_net_http_post_t *http_post);

static void reset_for_next_send(xt_net_http_post_t *http_post);

static void send_messages_get_new_message(xt_net_http_post_t *http_post);

static xt_core_bool_t send_messages_send_current_message
(xt_net_http_post_t *http_post);

char *get_header_lines(xt_net_http_message_t *http_message,
    unsigned short *header_lines_size)
{
  assert(http_message);
  assert(header_lines_size);
  xt_case_set_t *http_headers;
  char *header_lines;
  xt_core_nameobject_t *header;
  char *header_name;
  char *header_value;
  unsigned short line_size;
  unsigned short header_lines_position;
  unsigned short header_name_size;
  unsigned short header_value_size;

  header_lines = NULL;
  header_lines_position = 0;
  *header_lines_size = 0;
  http_headers = xt_net_http_message_get_http_headers(http_message);

  xt_case_set_iterate_start(http_headers);

  while ((header = xt_case_set_iterate_next(http_headers))) {

    header_name = xt_core_nameobject_get_name(header);
    header_value = xt_core_nameobject_get_object(header);
    header_name_size = strlen(header_name);
    header_value_size = strlen(header_value);
    line_size = header_name_size + 2 + header_value_size;

    *header_lines_size += line_size + 2;
    header_lines = realloc(header_lines, *header_lines_size);
    if (!header_lines) {
      xt_core_trace("realloc");
      break;
    }

    memcpy(header_lines + header_lines_position, header_name,
        header_name_size);
    header_lines_position += header_name_size;

    memcpy(header_lines + header_lines_position, ": ", 2);
    header_lines_position += 2;

    memcpy(header_lines + header_lines_position, header_value,
        header_value_size);
    header_lines_position += header_value_size;

    memcpy(header_lines + header_lines_position, "\r\n", 2);
    header_lines_position += 2;

  }

  return header_lines;
}

char *get_outgoing_status_line(xt_net_http_message_t *http_message,
    unsigned short *status_line_size)
{
  assert(http_message);
  assert(status_line_size);
  char *status_line;
  const char *http_version_name;
  xt_net_http_version_t http_version;
  unsigned short http_version_name_size;
  const char *status_string;
  unsigned short status_string_size;
  xt_net_http_status_t http_status;

  http_status = xt_net_http_message_get_http_status(http_message);
  http_version = xt_net_http_message_get_http_version(http_message);
  http_version_name = xt_net_http_version_get_name(http_version);
  http_version_name_size = strlen(http_version_name);

  status_string = get_outgoing_status_line_status_string(http_status,
      &status_string_size);

  *status_line_size = http_version_name_size + 1 + status_string_size + 2;
  status_line = malloc(*status_line_size);
  if (status_line) {
    memcpy(status_line, http_version_name, http_version_name_size);
    *(status_line + http_version_name_size) = ' ';
    memcpy(status_line + http_version_name_size + 1, status_string,
        status_string_size);
    memcpy(status_line + http_version_name_size + 1 + status_string_size,
        "\r\n", 2);
  } else {
    xt_core_trace("malloc");
  }

  return status_line;
}

const char *get_outgoing_status_line_status_string
(xt_net_http_status_t http_status, unsigned short *status_string_size)
{
  assert(status_string_size);
  const char *status_string;

  switch (http_status) {

    case XT_NET_HTTP_STATUS_OK:
      status_string = STATUS_STRING_OK;
      *status_string_size = strlen(STATUS_STRING_OK);
      break;

    case XT_NET_HTTP_STATUS_NOT_FOUND:
      status_string = STATUS_STRING_NOT_FOUND;
      *status_string_size = strlen(STATUS_STRING_NOT_FOUND);
      break;

    default:
    case XT_NET_HTTP_STATUS_UNKNOWN:
      xt_core_trace("invalid http status");
      status_string = "";
      *status_string_size = 0;
      break;

  }

  assert(status_string);
  return status_string;
}

int xt_net_http_post_compare(void *http_post_object_a,
    void *http_post_object_b)
{
  assert(http_post_object_a);
  assert(http_post_object_b);
  xt_net_http_post_t *http_post_a;
  xt_net_http_post_t *http_post_b;
  int compare;

  http_post_a = http_post_object_a;
  http_post_b = http_post_object_b;

  if (http_post_a->socket < http_post_b->socket) {
    compare = -1;
  } else if (http_post_a->socket > http_post_b->socket) {
    compare = 1;
  } else {
    compare = 0;
  }

  return compare;
}

xt_core_bool_t xt_net_http_post_compare_equal(void *http_post_object_a,
    void *http_post_object_b)
{
  assert(http_post_object_a);
  assert(http_post_object_b);
  xt_net_http_post_t *http_post_a = http_post_object_a;
  xt_net_http_post_t *http_post_b = http_post_object_b;
  xt_core_bool_t compare;

  if (http_post_a->socket == http_post_b->socket) {
    compare = xt_core_bool_true;
  } else {
    compare = xt_core_bool_false;
  }

  return compare;
}

void *xt_net_http_post_create(int socket)
{
  xt_net_http_post_t *http_post;
  xt_core_bool_t so_far_so_good;

  http_post = malloc(sizeof *http_post);
  if (http_post) {
    http_post->socket = socket;
    http_post->last_receive_activity_time = time(NULL);
    http_post->socket_closed = xt_core_bool_false;

    http_post->in_buffer_have_status_line = xt_core_bool_false;
    http_post->in_buffer_have_headers = xt_core_bool_false;
    http_post->in_buffer_have_body = xt_core_bool_false;
    http_post->in_buffer_have_complete_message = xt_core_bool_false;
    http_post->in_buffer_receive_position = 0;
    http_post->in_buffer_parse_position = 0;
    http_post->in_resource_path = NULL;
    http_post->in_body = NULL;

    http_post->out_buffer = NULL;
    http_post->out_buffer_size = 0;
    http_post->currently_sending_out_buffer = xt_core_bool_false;
    http_post->out_buffer_send_position = 0;

    xt_net_post_stats_init(&http_post->stats);

    so_far_so_good = xt_core_bool_true;
  } else {
    so_far_so_good = xt_core_bool_false;
    xt_core_trace("malloc");
  }

  if (so_far_so_good) {
    xt_core_nameobject_init_iobject(&http_post->nameobject_iobject);
    http_post->in_http_headers
      = xt_case_set_create(&http_post->nameobject_iobject);
    if (!http_post->in_http_headers) {
      xt_core_trace("x_case_set_create");
      so_far_so_good = xt_core_bool_false;
    }
  }

  if (so_far_so_good) {
    http_post->inbox = xt_case_list_create(XT_CORE_OBJECT_NO_COMPARE_F,
        XT_CORE_OBJECT_NO_COPY_F, XT_CORE_OBJECT_NO_DESTROY_F);
    if (!http_post->inbox) {
      so_far_so_good = xt_core_bool_false;
    }
  }

  if (so_far_so_good) {
    http_post->outbox = xt_case_list_create(XT_CORE_OBJECT_NO_COMPARE_F,
        XT_CORE_OBJECT_NO_COPY_F, xt_net_http_message_destroy);
    if (!http_post->outbox) {
      so_far_so_good = xt_core_bool_false;
    }
  }

  if (!so_far_so_good && http_post) {
    xt_net_http_post_create_rollback(http_post);
    http_post = NULL;
  }

  return http_post;
}

void *xt_net_http_post_create_decoy(int socket)
{
  xt_net_http_post_t *http_post;

  http_post = malloc(sizeof *http_post);
  if (http_post) {
    http_post->socket = socket;
  } else {
    xt_core_trace("malloc() failed\n");
  }

  return http_post;
}

void xt_net_http_post_create_rollback(xt_net_http_post_t *http_post)
{
  assert(http_post);

  if (http_post->inbox) {
    xt_case_list_destroy(http_post->inbox);
  }
  if (http_post->outbox) {
    xt_case_list_destroy(http_post->outbox);
  }
  free(http_post);
}

void xt_net_http_post_destroy(void *http_post_object)
{
  assert(http_post_object);
  xt_net_http_post_t *http_post;
  xt_net_http_message_t *http_message;

  http_post = http_post_object;

  xt_case_list_iterate_start(http_post->inbox);
  while ((http_message = xt_case_list_iterate_next(http_post->inbox))) {
    xt_net_http_message_destroy(http_message);
  }
  xt_case_list_destroy(http_post->inbox);

  xt_case_list_destroy(http_post->outbox);

  xt_case_set_destroy(http_post->in_http_headers);

  if (http_post->in_resource_path) {
    free(http_post->in_resource_path);
  }

  free(http_post);
}

void xt_net_http_post_destroy_decoy(void *http_post_object)
{
  free(http_post_object);
}

time_t xt_net_http_post_get_last_receive_activity_time
(void *http_post_object)
{
  assert(http_post_object);
  xt_net_http_post_t *http_post;

  http_post = http_post_object;

  return http_post->last_receive_activity_time;
}

int xt_net_http_post_get_socket(void *http_post_object)
{
  assert(http_post_object);
  xt_net_http_post_t *http_post;

  http_post = http_post_object;

  return http_post->socket;
}

void xt_net_http_post_get_stats(void *http_post_object,
    xt_net_post_stats_t *post_stats)
{
  assert(http_post_object);
  assert(post_stats);
  xt_net_http_post_t *http_post;

  http_post = http_post_object;

  post_stats->messages_sent = http_post->stats.messages_sent;
  post_stats->messages_received = http_post->stats.messages_received;
  post_stats->messages_in_inbox = http_post->stats.messages_in_inbox;
  post_stats->messages_in_outbox = http_post->stats.messages_in_outbox;
  post_stats->most_messages_ever_in_inbox
    = http_post->stats.most_messages_ever_in_inbox;
  post_stats->most_messages_ever_in_outbox
    = http_post->stats.most_messages_ever_in_outbox;
  post_stats->send_message_failures = http_post->stats.send_message_failures;
  post_stats->receive_message_failures
    = http_post->stats.receive_message_failures;
  post_stats->messages_not_sent_due_to_socket_send_failures
    = http_post->stats.messages_not_sent_due_to_socket_send_failures;
}

xt_core_bool_t xt_net_http_post_is_socket_closed(void *http_post_object)
{
  assert(http_post_object);
  xt_net_http_post_t *http_post;

  http_post = http_post_object;

  return http_post->socket_closed;
}

unsigned long xt_net_http_post_mod(void *post_object, unsigned long divisor)
{
  assert(post_object);
  xt_net_http_post_t *post = post_object;
  return post->socket % divisor;
}

void *xt_net_http_post_receive_message(void *http_post_object)
{
  assert(http_post_object);
  xt_net_http_post_t *http_post;
  xt_net_http_message_t *http_message;

  http_post = http_post_object;

  http_message = xt_case_list_find_first(http_post->inbox);
  if (http_message) {
    xt_case_list_remove_first(http_post->inbox);
  }

  return http_message;
}

void xt_net_http_post_receive_messages(void *http_post_object)
{
  assert(http_post_object);
  xt_net_http_post_t *http_post;

  http_post = http_post_object;

  http_post->last_receive_activity_time = time(NULL);

  if (receive_messages(http_post)) {
    reset_for_next_receive(http_post);
  }
}

xt_core_bool_t xt_net_http_post_send_message(void *http_post_object,
    void *http_message_object)
{
  assert(http_post_object);
  assert(http_message_object);
  xt_core_bool_t success;
  xt_net_http_post_t *http_post;

  http_post = http_post_object;

  success = xt_case_list_add_last(http_post->outbox, http_message_object);

  return success;
}

void xt_net_http_post_send_messages(void *http_post_object)
{
  assert(http_post_object);
  xt_net_http_post_t *http_post;

  http_post = http_post_object;

  if (!http_post->currently_sending_out_buffer) {
    send_messages_get_new_message(http_post);
  }
  if (http_post->currently_sending_out_buffer) {
    if (send_messages_send_current_message(http_post)) {
      reset_for_next_send(http_post);
    }
  }
}

xt_net_http_method_t parse_http_method(char *resource_path_string)
{
  assert(resource_path_string);
  xt_net_http_method_t http_method;

  if (0 == strcmp(resource_path_string, "GET")) {
    http_method = XT_NET_HTTP_METHOD_GET;

  } else if (0 == strcmp(resource_path_string, "HEAD")) {
    http_method = XT_NET_HTTP_METHOD_HEAD;

  } else if (0 == strcmp(resource_path_string, "POST")) {
    http_method = XT_NET_HTTP_METHOD_POST;

  } else {
    http_method = XT_NET_HTTP_METHOD_UNKNOWN;

  }

  return http_method;
}

xt_net_http_version_t parse_http_version(char *http_version_string)
{
  assert(http_version_string);
  xt_net_http_version_t http_version;

  if (0 == strcmp(http_version_string, "HTTP/1.1")) {
    http_version = XT_NET_HTTP_VERSION_1_1;

  } else if (0 == strcmp(http_version_string, "HTTP/1.0")) {
    http_version = XT_NET_HTTP_VERSION_1_0;

  } else {
    http_version = XT_NET_HTTP_VERSION_UNKNOWN;

  }

  return http_version;
}

void parse_incoming_message(xt_net_http_post_t *http_post)
{
  assert(http_post);
  assert(!http_post->in_buffer_have_complete_message);

  if (!http_post->in_buffer_have_status_line) {
    parse_incoming_message_status_line(http_post);
  }

  if (http_post->in_buffer_have_status_line
      && !http_post->in_buffer_have_headers) {
    parse_incoming_message_headers(http_post);
  }

  if (http_post->in_buffer_have_status_line
      && http_post->in_buffer_have_headers
      && http_post->in_buffer_expecting_body
      && !http_post->in_buffer_have_body) {
    parse_incoming_message_body(http_post);
  }
}

void parse_incoming_message_status_line(xt_net_http_post_t *http_post)
{
  assert(http_post);
  char *first_newline;
  char *remaining_message;
  unsigned short line_size;
  char *line;
  char *http_method_string;
  char *resource_path_string;
  char *http_version_string;
  char *strtok_r_char;

  strtok_r_char = NULL;

  remaining_message = http_post->in_buffer
    + http_post->in_buffer_parse_position;

  first_newline = strstr(remaining_message, "\r\n");
  if (first_newline) {
    line_size = first_newline - http_post->in_buffer;
    line = malloc(line_size + 1);
    if (line) {
      memcpy(line, http_post->in_buffer, line_size);
      *(line + line_size) = '\0';

      http_method_string = strtok_r(line, " ", &strtok_r_char);
      resource_path_string = strtok_r(NULL, " ", &strtok_r_char);
      http_version_string = strtok_r(NULL, " ", &strtok_r_char);

      if (http_method_string && resource_path_string && http_version_string) {
        http_post->in_http_method = parse_http_method(http_method_string);
        http_post->in_resource_path = strdup(resource_path_string);
        http_post->in_http_version = parse_http_version(http_version_string);
        http_post->in_buffer_have_status_line = xt_core_bool_true;
        http_post->in_buffer_parse_position += line_size + 2;
      } else {
        printf("http_post received malformed http status line from "
            "client %i\n", http_post->socket);
      }

      free(line);
    } else {
      xt_core_trace("malloc");
    }
  }
}

void parse_incoming_message_headers(xt_net_http_post_t *http_post)
{
  char *double_newline_char;
  char *start_char;
  char *line;
  char *name;
  char *value;
  char *line_context;
  char *nameobject_context;
  xt_core_nameobject_t *nameobject;

  start_char = http_post->in_buffer + http_post->in_buffer_parse_position;
  double_newline_char = strstr(start_char, "\r\n\r\n");
  if (double_newline_char) {
    http_post->in_buffer_expecting_body = xt_core_bool_false;
    line = strtok_r(start_char, "\r\n", &line_context);
    while (line) {
      name = strtok_r(line, ": ", &nameobject_context);
      value = strtok_r(NULL, ": ", &nameobject_context);
      if (name && value) {
        if ((0 == strcmp(name, "Content-Length"))
            || (0 == strcmp(name, "Transfer-Encoding"))) {
          http_post->in_buffer_expecting_body = xt_core_bool_true;
        }
        nameobject = xt_core_nameobject_create(name, value, xt_core_string_copy,
            xt_core_string_destroy, xt_core_string_get_as_string);
        if (nameobject) {
          if (!xt_case_set_add(http_post->in_http_headers, nameobject)) {
            xt_core_nameobject_destroy(nameobject);
          }
        }
      }
      line = strtok_r(NULL, "\r\n", &line_context);
    }

    if (!http_post->in_buffer_expecting_body) {
      http_post->in_buffer_have_complete_message = xt_core_bool_true;
    }

    http_post->in_buffer_have_headers = xt_core_bool_true;
    http_post->in_buffer_parse_position
      += (double_newline_char - start_char) + 2;
  }
}

void parse_incoming_message_body(xt_net_http_post_t *http_post)
{
  /*
    TODO: if that works, set http_post->in_buffer_have_complete_message
  */
}

xt_core_bool_t put_message_into_out_buffer(xt_net_http_post_t *http_post,
    xt_net_http_message_t *http_message)
{
  assert(http_post);
  assert(http_message);
  xt_core_bool_t success;
  unsigned long message_body_size;
  char *message_body;
  char *status_line;
  unsigned short status_line_size;
  char *headers;
  unsigned short headers_size;
  unsigned long message_size;
  unsigned long buffer_write_position;

  status_line = get_outgoing_status_line(http_message, &status_line_size);
  if (status_line) {
    headers = get_header_lines(http_message, &headers_size);

    message_body = xt_net_http_message_get_body(http_message);
    message_body_size = xt_net_http_message_get_body_size(http_message);

    message_size = status_line_size + headers_size + 2 + message_body_size;

    http_post->out_buffer = malloc(message_size);
    if (http_post->out_buffer) {
      success = xt_core_bool_true;
      buffer_write_position = 0;

      memcpy(http_post->out_buffer + buffer_write_position, status_line,
          status_line_size);
      buffer_write_position += status_line_size;

      memcpy(http_post->out_buffer + buffer_write_position, headers,
          headers_size);
      buffer_write_position += headers_size;
      memcpy(http_post->out_buffer + buffer_write_position, "\r\n", 2);
      buffer_write_position += 2;

      memcpy(http_post->out_buffer + buffer_write_position, message_body,
          message_body_size);

      http_post->out_buffer_size = message_size;
      http_post->out_buffer_send_position = 0;
    } else {
      xt_core_trace("malloc");
      success = xt_core_bool_false;
    }

    free(status_line);
    if (headers) {
      free(headers);
    }
  } else {
    xt_core_trace("get_outgoing_status_line");
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t put_received_message_in_inbox(xt_net_http_post_t *http_post)
{
  xt_core_bool_t success;
  xt_net_http_message_t *http_message;

  http_message = xt_net_http_message_create(http_post->socket,
      http_post->in_http_method, XT_NET_HTTP_STATUS_UNKNOWN,
      http_post->in_resource_path, http_post->in_http_version,
      http_post->in_http_headers);
  if (http_message) {
    if (xt_case_list_add_last(http_post->inbox, http_message)) {
      success = xt_core_bool_true;
    } else {
      xt_core_trace("x_case_list_add_last");
      success = xt_core_bool_false;
    }
  } else {
    xt_core_trace("x_net_http_message_create");
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t receive_messages(xt_net_http_post_t *http_post)
{
  assert(http_post);
  xt_core_bool_t received_complete_message;
  int actual_bytes_read;
  int max_bytes_to_read;

  received_complete_message = xt_core_bool_false;

  max_bytes_to_read = IN_BUFFER_SIZE - http_post->in_buffer_receive_position;
  actual_bytes_read = xt_net_socket_receive(http_post->socket,
      http_post->in_buffer + http_post->in_buffer_receive_position,
      max_bytes_to_read);
  if (actual_bytes_read > 0) {
    http_post->in_buffer_receive_position += actual_bytes_read;
    parse_incoming_message(http_post);
    if (http_post->in_buffer_have_complete_message) {
      received_complete_message = xt_core_bool_true;
      if (!put_received_message_in_inbox(http_post)) {
        xt_core_trace("put_received_message_in_inbox");
      }
    }
  } else {
    http_post->socket_closed = xt_core_bool_true;
    received_complete_message = xt_core_bool_true;
  }

  return received_complete_message;
}

void reset_for_next_receive(xt_net_http_post_t *http_post)
{
  http_post->in_buffer_have_status_line = xt_core_bool_false;
  http_post->in_buffer_have_headers = xt_core_bool_false;
  http_post->in_buffer_have_body = xt_core_bool_false;
  http_post->in_buffer_have_complete_message = xt_core_bool_false;
  http_post->in_buffer_receive_position = 0;
  http_post->in_buffer_parse_position = 0;
  if (http_post->in_resource_path) {
    free(http_post->in_resource_path);
    http_post->in_resource_path = NULL;
  }
  xt_case_set_clear(http_post->in_http_headers);
  if (http_post->in_body) {
    free(http_post->in_body);
    http_post->in_body = NULL;
  }
}

void reset_for_next_send(xt_net_http_post_t *http_post)
{
  free(http_post->out_buffer);
  http_post->out_buffer = NULL;
  http_post->out_buffer_size = 0;
  http_post->currently_sending_out_buffer = xt_core_bool_false;
  http_post->out_buffer_send_position = 0;
}

void send_messages_get_new_message(xt_net_http_post_t *http_post)
{
  xt_net_http_message_t *http_message;

  http_message = xt_case_list_find_first(http_post->outbox);
  if (http_message) {
    if (put_message_into_out_buffer(http_post, http_message)) {
      xt_case_list_remove_first(http_post->outbox);
      http_post->currently_sending_out_buffer = xt_core_bool_true;
    } else {
      xt_core_trace("put_message_into_out_buffer");
    }
  }
}

xt_core_bool_t send_messages_send_current_message(xt_net_http_post_t *http_post)
{
  assert(http_post);
  xt_core_bool_t sent_complete_message;
  int actual_bytes_sent;
  int bytes_remaining_to_send;

  bytes_remaining_to_send = http_post->out_buffer_size
    - http_post->out_buffer_send_position;
  if (bytes_remaining_to_send > 0) {
    actual_bytes_sent = xt_net_socket_send(http_post->socket,
        http_post->out_buffer, bytes_remaining_to_send);
    if (actual_bytes_sent > 0) {
      http_post->out_buffer_send_position += actual_bytes_sent;
      if (http_post->out_buffer_send_position == http_post->out_buffer_size) {
        sent_complete_message = xt_core_bool_true;
      } else {
        sent_complete_message = xt_core_bool_false;
      }
    } else {
      http_post->socket_closed = xt_core_bool_true;
      sent_complete_message = xt_core_bool_false;
    }
  } else {
    sent_complete_message = xt_core_bool_true;
  }

  return sent_complete_message;
}
