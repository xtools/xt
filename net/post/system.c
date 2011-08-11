#include "xt/case/list.h"
#include "xt/core/tools.h"
#include "xt/net/post/system.h"
#include "xt/net/socket.h"

/*  TODO: add support for message encodings  */

#define POST_MESSAGE_MAGIC 19780112

struct post_message_header_t {
  long int magic;
  short int encoding;
  short int engine_id;
  long int message_type;
  long int data_size;
};
typedef struct post_message_header_t post_message_header_t;

struct xt_net_post_system_t {
  int socket;
  xt_case_list_t *inbox;
  xt_case_list_t *outbox;
  post_message_header_t current_message_header;
  char *current_message_data;
  xt_core_bool_t have_message_header;
  xt_net_post_stats_t stats;
  unsigned long magic;

  char *out_buffer;
  unsigned long out_buffer_size;
  xt_core_bool_t currently_sending_out_buffer;
  unsigned long out_buffer_send_position;

  char *in_buffer;
  xt_core_bool_t in_buffer_have_complete_header;
  unsigned long in_buffer_receive_position;
  post_message_header_t in_message_header;

  time_t last_receive_activity_time;
  xt_core_bool_t socket_closed;
};

static void xt_net_post_system_create_in_buffer(xt_net_post_system_t *post);

static xt_core_bool_t xt_net_post_system_create_inbox(xt_net_post_system_t *post);

static void xt_net_post_system_create_out_buffer(xt_net_post_system_t *post);

static xt_core_bool_t xt_net_post_system_create_outbox(xt_net_post_system_t *post);

static void xt_net_post_system_create_rollback(xt_net_post_system_t *post);

static void init_post_message_header(xt_net_post_system_t *post,
    post_message_header_t *post_message_header,
    xt_core_message_encoding_t encoding, xt_net_engine_id_t engine_id,
    unsigned long message_type, unsigned long data_size);

static void ntox_post_message_header
(post_message_header_t *post_message_header);

static xt_core_bool_t put_message_into_out_buffer(xt_net_post_system_t *post,
    xt_core_message_t *message);

static xt_core_bool_t put_received_message_in_inbox(xt_net_post_system_t *post);

static xt_core_bool_t receive_messages_body(xt_net_post_system_t *post);

static xt_core_bool_t receive_messages_header(xt_net_post_system_t *post);

static void reset_for_next_receive(xt_net_post_system_t *post);

static void reset_for_next_send(xt_net_post_system_t *post);

static void send_messages_get_new_message(xt_net_post_system_t *post);

static xt_core_bool_t send_messages_send_current_message(xt_net_post_system_t *post);

int xt_net_post_system_compare(void *post_object_a, void *post_object_b)
{
  assert(post_object_a);
  assert(post_object_b);
  int compare_result;
  xt_net_post_system_t *post_a;
  xt_net_post_system_t *post_b;

  post_a = post_object_a;
  post_b = post_object_b;

  if (post_a->socket == post_b->socket) {
    compare_result = 0;
  } else if (post_a->socket < post_b->socket) {
    compare_result = -1;
  } else {
    compare_result = 1;
  }

  return compare_result;
}

xt_core_bool_t xt_net_post_system_compare_equal(void *post_object_a,
    void *post_object_b)
{
  assert(post_object_a);
  assert(post_object_b);
  xt_net_post_system_t *post_a = post_object_a;
  xt_net_post_system_t *post_b = post_object_b;
  xt_core_bool_t compare;

  if (post_a->socket == post_b->socket) {
    compare = xt_core_bool_true;
  } else {
    compare = xt_core_bool_false;
  }

  return compare;
}

void *xt_net_post_system_create(int socket)
{
  xt_net_post_system_t *post;
  xt_core_bool_t success;

  post = malloc(sizeof *post);
  if (post) {
    post->socket = socket;
    post->have_message_header = xt_core_bool_false;
    post->magic = htonl(POST_MESSAGE_MAGIC);
    post->currently_sending_out_buffer = xt_core_bool_false;
    post->last_receive_activity_time = time(NULL);
    xt_net_post_stats_init(&post->stats);
    xt_net_post_system_create_in_buffer(post);
    xt_net_post_system_create_out_buffer(post);
    post->socket_closed = xt_core_bool_false;
    success = xt_core_bool_true;
  } else {
    xt_core_trace("malloc");
    success = xt_core_bool_false;
  }

  if (success) {
    success = xt_net_post_system_create_inbox(post);
  }

  if (success) {
    success = xt_net_post_system_create_outbox(post);
  }

  if (!success) {
    xt_net_post_system_create_rollback(post);
    post = NULL;
  }

  return post;
}

void *xt_net_post_system_create_decoy(int socket)
{
  xt_net_post_system_t *post;

  post = malloc(sizeof *post);
  if (post) {
    post->socket = socket;
  } else {
    xt_core_trace("malloc");
  }

  return post;
}

void xt_net_post_system_create_in_buffer(xt_net_post_system_t *post)
{
  post->in_buffer = NULL;
  post->in_buffer_have_complete_header = xt_core_bool_false;
  post->in_buffer_receive_position = 0;
}

xt_core_bool_t xt_net_post_system_create_inbox(xt_net_post_system_t *post)
{
  xt_core_bool_t success;

  post->inbox = xt_case_list_create(XT_CORE_OBJECT_NO_COMPARE_F,
      xt_core_message_copy, XT_CORE_OBJECT_NO_DESTROY_F);
  if (post->inbox) {
    success = xt_core_bool_true;
  } else {
    xt_core_trace("x_case_list_create");
    success = xt_core_bool_false;
  }

  return success;
}

void xt_net_post_system_create_out_buffer(xt_net_post_system_t *post)
{
  post->out_buffer = NULL;
  post->currently_sending_out_buffer = xt_core_bool_false;
  post->out_buffer_send_position = 0;
}

xt_core_bool_t xt_net_post_system_create_outbox(xt_net_post_system_t *post)
{
  xt_core_bool_t success;

  post->outbox = xt_case_list_create(XT_CORE_OBJECT_NO_COMPARE_F,
      xt_core_message_copy, xt_core_message_destroy);
  if (post->outbox) {
    success = xt_core_bool_true;
  } else {
    xt_core_trace("x_case_list_create");
    success = xt_core_bool_false;
  }

  return success;
}

void xt_net_post_system_create_rollback(xt_net_post_system_t *post)
{
  if (post) {
    if (post->in_buffer) {
      free(post->in_buffer);
    }
    if (post->inbox) {
      xt_case_list_destroy(post->inbox);
    }
    if (post->outbox) {
      xt_case_list_destroy(post->outbox);
    }
  }
}

void xt_net_post_system_destroy(void *post_object)
{
  assert(post_object);
  xt_net_post_system_t *post;
  xt_core_message_t *message;

  post = post_object;

  xt_case_list_iterate_start(post->inbox);
  while ((message = xt_case_list_iterate_next(post->inbox))) {
    xt_core_message_destroy(message);
  }
  xt_case_list_destroy(post->inbox);

  xt_case_list_destroy(post->outbox);

  if (post->in_buffer) {
    free(post->in_buffer);
  }
  if (post->out_buffer) {
    free(post->out_buffer);
  }

  free(post);
}

void xt_net_post_system_destroy_decoy(void *post_object)
{
  free(post_object);
}

time_t xt_net_post_system_get_last_receive_activity_time(void *post_object)
{
  assert(post_object);
  xt_net_post_system_t *post;

  post = post_object;

  return post->last_receive_activity_time;
}

int xt_net_post_system_get_socket(void *post_object)
{
  assert(post_object);
  xt_net_post_system_t *post;

  post = post_object;

  return post->socket;
}

void xt_net_post_system_get_stats(void *post_object,
    xt_net_post_stats_t *post_stats)
{
  assert(post_object);
  assert(post_stats);
  xt_net_post_system_t *post;

  post = post_object;

  post_stats->messages_sent = post->stats.messages_sent;
  post_stats->messages_received = post->stats.messages_received;
  post_stats->messages_in_inbox = post->stats.messages_in_inbox;
  post_stats->messages_in_outbox = post->stats.messages_in_outbox;
  post_stats->most_messages_ever_in_inbox
    = post->stats.most_messages_ever_in_inbox;
  post_stats->most_messages_ever_in_outbox
    = post->stats.most_messages_ever_in_outbox;
  post_stats->send_message_failures = post->stats.send_message_failures;
  post_stats->receive_message_failures
    = post->stats.receive_message_failures;
  post_stats->messages_not_sent_due_to_socket_send_failures
    = post->stats.messages_not_sent_due_to_socket_send_failures;
}

xt_core_bool_t xt_net_post_system_is_socket_closed(void *post_object)
{
  assert(post_object);
  xt_net_post_system_t *post;

  post = post_object;

  return post->socket_closed;
}

unsigned long xt_net_post_system_mod(void *post_object, unsigned long divisor)
{
  assert(post_object);
  xt_net_post_system_t *post = post_object;
  return post->socket % divisor;
}

void *xt_net_post_system_receive_message(void *post_object)
{
  assert(post_object);
  xt_net_post_system_t *post;
  xt_core_message_t *message;

  post = post_object;

  message = xt_case_list_find_first(post->inbox);
  if (message) {
    xt_case_list_remove_first(post->inbox);
  }

  return message;
}

void xt_net_post_system_receive_messages(void *post_object)
{
  assert(post_object);
  xt_net_post_system_t *post;

  post = post_object;

  post->last_receive_activity_time = time(NULL);

  if (!post->in_buffer_have_complete_header) {
    if (receive_messages_header(post)) {
      post->in_buffer_have_complete_header = xt_core_bool_true;
    }
  }
  if (post->in_buffer_have_complete_header) {
    if (receive_messages_body(post)) {
      reset_for_next_receive(post);
    }
  }
}

xt_core_bool_t xt_net_post_system_send_message(void *post_object,
    void *message_object)
{
  assert(post_object);
  assert(message_object);
  xt_core_bool_t success;
  xt_net_post_system_t *post;

  post = post_object;

  success = xt_case_list_add_last(post->outbox, message_object);

  return success;
}

void xt_net_post_system_send_messages(void *post_object)
{
  assert(post_object);
  xt_net_post_system_t *post;

  post = post_object;

  if (!post->currently_sending_out_buffer) {
    send_messages_get_new_message(post);
  }
  if (post->currently_sending_out_buffer) {
    if (send_messages_send_current_message(post)) {
      reset_for_next_send(post);
    }
  }
}

xt_case_list_t *xt_net_post_system_take_unsent_messages(xt_net_post_system_t *post)
{
  assert(post);
  xt_case_list_t *list;
  xt_core_message_t *message;

  list = xt_case_list_create(XT_CORE_OBJECT_NO_COMPARE_F,
      xt_core_message_copy, XT_CORE_OBJECT_NO_DESTROY_F);
  if (list) {
    xt_case_list_iterate_start(post->outbox);
    while ((message = xt_case_list_iterate_next(post->outbox))) {
      if (xt_case_list_add_last(list, message)) {
        xt_case_list_iterate_remove(post->outbox);
      } else {
        xt_core_trace("x_case_list_add_last");
      }
    }
  } else {
    xt_core_trace("x_case_list_create");
  }

  return list;
}

void init_post_message_header(xt_net_post_system_t *post,
    post_message_header_t *post_message_header,
    xt_core_message_encoding_t encoding, xt_net_engine_id_t engine_id,
    unsigned long message_type, unsigned long data_size)
{
  post_message_header->magic = post->magic;
  post_message_header->encoding = htons(encoding);
  post_message_header->engine_id = htons(engine_id);
  post_message_header->message_type = htonl(message_type);
  post_message_header->data_size = htonl(data_size);
}

void ntox_post_message_header(post_message_header_t *post_message_header)
{
  post_message_header->magic = ntohl(post_message_header->magic);
  post_message_header->encoding = ntohs(post_message_header->encoding);
  post_message_header->engine_id = ntohs(post_message_header->engine_id);
  post_message_header->message_type = ntohl(post_message_header->message_type);
  post_message_header->data_size = ntohl(post_message_header->data_size);
}

xt_core_bool_t put_message_into_out_buffer(xt_net_post_system_t *post,
    xt_core_message_t *message)
{
  xt_core_bool_t success;
  unsigned long post_message_size;
  post_message_header_t post_message_header;
  unsigned long message_data_size;

  message_data_size = xt_core_message_get_data_get_size(message);
  init_post_message_header(post, &post_message_header,
      xt_core_message_get_encoding(message),
      xt_core_message_get_engine_id(message),
      xt_core_message_get_type(message), message_data_size);

  post_message_size = (sizeof post_message_header) + message_data_size;

  post->out_buffer = malloc(post_message_size);
  if (post->out_buffer) {
    memcpy(post->out_buffer, &post_message_header, sizeof post_message_header);
    memcpy(post->out_buffer + (sizeof post_message_header),
        xt_core_message_get_data(message), message_data_size);
    post->out_buffer_size = post_message_size;
    success = xt_core_bool_true;
  } else {
    xt_core_trace("malloc");
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t put_received_message_in_inbox(xt_net_post_system_t *post)
{
  xt_core_bool_t success;
  xt_core_message_t *message;

  message = xt_core_message_create(post->socket,
      post->in_message_header.engine_id, post->in_message_header.message_type,
      post->in_buffer, post->in_message_header.data_size);
  if (message) {
    if (xt_case_list_add_last(post->inbox, message)) {
      success = xt_core_bool_true;
    } else {
      xt_core_trace("x_case_list_add_last");
      success = xt_core_bool_false;
    }
  } else {
    xt_core_trace("x_core_message_create");
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t receive_messages_body(xt_net_post_system_t *post)
{
  assert(post);
  assert(post->in_buffer_have_complete_header);
  assert(post->in_buffer);
  xt_core_bool_t received_complete_message;
  unsigned long bytes_remaining_to_read;
  int actual_bytes_read;

  received_complete_message = xt_core_bool_false;

  bytes_remaining_to_read = post->in_message_header.data_size
    - post->in_buffer_receive_position;
  if (bytes_remaining_to_read > 0) {
    actual_bytes_read = xt_net_socket_receive(post->socket,
        post->in_buffer + post->in_buffer_receive_position,
        bytes_remaining_to_read);
    if (actual_bytes_read > 0) {
      post->in_buffer_receive_position += actual_bytes_read;
      if (post->in_message_header.data_size
          == post->in_buffer_receive_position) {
        received_complete_message = xt_core_bool_true;
      }
    } else {
      post->socket_closed = xt_core_bool_true;
    }
  } else {
    received_complete_message = xt_core_bool_true;
  }
  if (received_complete_message) {
    if (!put_received_message_in_inbox(post)) {
      xt_core_trace("put_received_message_in_inbox failed()");
    }
  }

  return received_complete_message;
}

xt_core_bool_t receive_messages_header(xt_net_post_system_t *post)
{
  unsigned long bytes_remaining_to_receive;
  int actual_bytes_read;
  xt_core_bool_t received_a_header;

  received_a_header = xt_core_bool_false;

  bytes_remaining_to_receive = (sizeof post->in_message_header)
    - post->in_buffer_receive_position;
  actual_bytes_read = xt_net_socket_receive(post->socket,
      &post->in_message_header + post->in_buffer_receive_position,
      bytes_remaining_to_receive);
  if (actual_bytes_read > 0) {
    post->in_buffer_receive_position += actual_bytes_read;
    if ((sizeof post->in_message_header) == post->in_buffer_receive_position) {
      ntox_post_message_header(&post->in_message_header);
      if (POST_MESSAGE_MAGIC != post->in_message_header.magic) {
        xt_core_trace("[post] message magic is incorrect\n");
      }
      received_a_header = xt_core_bool_true;
      post->in_buffer_receive_position = 0;
      post->in_buffer = malloc(post->in_message_header.data_size);
      if (!post->in_buffer) {
        xt_core_trace("malloc");
      }
    }
  } else {
    post->socket_closed = xt_core_bool_true;
  }

  return received_a_header;
}

void reset_for_next_receive(xt_net_post_system_t *post)
{
  free(post->in_buffer);
  post->in_buffer = NULL;
  post->in_buffer_have_complete_header = xt_core_bool_false;
  post->in_buffer_receive_position = 0;
}

void reset_for_next_send(xt_net_post_system_t *post)
{
  free(post->out_buffer);
  post->out_buffer = NULL;
  post->currently_sending_out_buffer = xt_core_bool_false;
  post->out_buffer_send_position = 0;
}

void send_messages_get_new_message(xt_net_post_system_t *post)
{
  xt_core_message_t *message;

  message = xt_case_list_find_first(post->outbox);
  if (message) {
    if (put_message_into_out_buffer(post, message)) {
      xt_case_list_remove_first(post->outbox);
      post->currently_sending_out_buffer = xt_core_bool_true;
    } else {
      xt_core_trace("put_message_into_out_buffer");
    }
  }
}

xt_core_bool_t send_messages_send_current_message(xt_net_post_system_t *post)
{
  xt_core_bool_t sent_complete_message;
  unsigned long bytes_remaining_to_send;
  int actual_bytes_sent;

  bytes_remaining_to_send = post->out_buffer_size
    - post->out_buffer_send_position;

  if (bytes_remaining_to_send > 0) {
    actual_bytes_sent = xt_net_socket_send(post->socket,
        post->out_buffer + post->out_buffer_send_position,
        bytes_remaining_to_send);
    if (actual_bytes_sent > 0) {
      post->out_buffer_send_position += actual_bytes_sent;
      if (post->out_buffer_send_position == post->out_buffer_size) {
        sent_complete_message = xt_core_bool_true;
      } else {
        sent_complete_message = xt_core_bool_false;
      }
    } else {
      post->socket_closed = xt_core_bool_true;
      sent_complete_message = xt_core_bool_false;
    }
  } else {
    sent_complete_message = xt_core_bool_true;
  }

  return sent_complete_message;
}
