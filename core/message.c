#include "xt/core/engine.h"
#include "xt/core/message.h"
#include "xt/core/string.h"
#include "xt/core/tools.h"

struct xt_core_message_t {
  int client_socket;
  xt_core_message_encoding_t encoding;
  xt_net_engine_id_t engine_id;
  unsigned long message_type;
  unsigned long data_size;
  char *data;
  unsigned long deserialize_position;
};

xt_core_bool_t xt_core_message_add_bool(xt_core_message_t *message,
    void *object)
{
  return xt_core_message_add_short(message, object);
}

xt_core_bool_t xt_core_message_add_double(xt_core_message_t *message,
    void *object)
{
  xt_core_bool_t success;
  char *double_string;
  int chars_written;
  double *double_pointer;
  char *new_data;

  double_pointer = object;
  chars_written = asprintf(&double_string, "%f", *double_pointer);
  if (chars_written >= 0) {
    new_data = realloc(message->data, message->data_size + chars_written + 1);
    if (new_data) {
      success = xt_core_bool_true;
      message->data = new_data;
      memcpy(message->data + message->data_size, double_string,
          chars_written);
      *(message->data + message->data_size + chars_written) = '\0';
      message->data_size += (chars_written + 1);
    } else {
      success = xt_core_bool_false;
      xt_core_trace("realloc");
    }
    free(double_string);
  } else {
    success = xt_core_bool_false;
    xt_core_trace("asprintf");
  }

  return success;
}

xt_core_bool_t xt_core_message_add_long(xt_core_message_t *message,
    void *object)
{
  xt_core_bool_t success;
  long *long_pointer;
  long long_object_network;
  char *new_data;

  long_pointer = object;
  long_object_network = htonl(*long_pointer);
  new_data = realloc(message->data, message->data_size
      + sizeof long_object_network);
  if (new_data) {
    message->data = new_data;
    memcpy(message->data + message->data_size, &long_object_network,
        sizeof long_object_network);
    message->data_size += sizeof long_object_network;
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_core_message_add_short(xt_core_message_t *message,
    void *object)
{
  xt_core_bool_t success;
  short *short_pointer;
  short short_object_network;
  char *new_data;

  short_pointer = object;
  short_object_network = htons(*short_pointer);
  new_data = realloc(message->data, message->data_size
      + sizeof short_object_network);
  if (new_data) {
    message->data = new_data;
    memcpy(message->data + message->data_size, &short_object_network,
        sizeof short_object_network);
    message->data_size += sizeof short_object_network;
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_core_message_add_string(xt_core_message_t *message,
    void *object)
{
  xt_core_bool_t success;
  char *new_data;
  size_t string_size_with_terminator;

  string_size_with_terminator = strlen(object) + 1;
  new_data = realloc(message->data, message->data_size
      + string_size_with_terminator);
  if (new_data) {
    message->data = new_data;
    memcpy(message->data + message->data_size, object,
        string_size_with_terminator);
    message->data_size += string_size_with_terminator;
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
    xt_core_trace("realloc");
  }

  return success;
}

void *xt_core_message_copy(void *message_object)
{
  assert(message_object);
  xt_core_message_t *message;
  xt_core_message_t *message_copy;

  message = message_object;

  message_copy = malloc(sizeof *message_copy);
  if (message_copy) {
    message_copy->client_socket = message->client_socket;
    message_copy->encoding = message->encoding;
    message_copy->engine_id = message->engine_id;
    message_copy->message_type = message->message_type;
    message_copy->data_size = message->data_size;
    message_copy->deserialize_position = message->deserialize_position;
    message_copy->data = malloc(message->data_size);
    if (message_copy->data) {
      memcpy(message_copy->data, message->data, message->data_size);
    } else {
      xt_core_trace("malloc");
    }
  } else {
    xt_core_trace("malloc");
  }

  return message_copy;
}

xt_core_message_t *xt_core_message_create(int client_socket,
    xt_net_engine_id_t engine_id, unsigned long message_type, char *data,
    unsigned long data_size)
{
  xt_core_message_t *message;

  message = malloc(sizeof *message);
  if (message) {
    message->client_socket = client_socket;
    message->encoding = XT_CORE_MESSAGE_ENCODING_PLAIN;
    message->engine_id = engine_id;
    message->message_type = message_type;
    message->deserialize_position = 0;
    message->data_size = data_size;
    if (data_size != 0) {
      message->data = malloc(data_size);
      if (message->data) {
        memcpy(message->data, data, data_size);
      } else {
        free(message);
        message = NULL;
      }
    } else {
      message->data = NULL;
    }
  } else {
    xt_core_trace("malloc");
  }

  return message;
}

void xt_core_message_destroy(void *message_object)
{
  assert(message_object);
  xt_core_message_t *message;

  message = message_object;

  if (message->data) {
    free(message->data);
  }
  free(message);

}

void xt_core_message_free(void *message_object)
{
  xt_core_message_t *message;

  message = message_object;
  xt_core_message_destroy(message);
}

int xt_core_message_get_client_socket(void *message_object)
{
  assert(message_object);
  xt_core_message_t *message;

  message = message_object;

  return message->client_socket;
}

char *xt_core_message_get_data(xt_core_message_t *message)
{
  return message->data;
}

unsigned long xt_core_message_get_data_get_size(xt_core_message_t *message)
{
  return message->data_size;
}

xt_core_message_encoding_t xt_core_message_get_encoding
(xt_core_message_t *message)
{
  return message->encoding;
}

xt_net_engine_id_t xt_core_message_get_engine_id(void *message_object)
{
  assert(message_object);
  xt_core_message_t *message;

  message = message_object;

  return message->engine_id;
}

unsigned long xt_core_message_get_type(void *message_object)
{
  assert(message_object);
  xt_core_message_t *message;

  message = message_object;

  return message->message_type;
}

void *xt_core_message_take_bool(xt_core_message_t *message)
{
  return xt_core_message_take_short(message);
}

xt_core_bool_t xt_core_message_take_bool_value(xt_core_message_t *message)
{
  return xt_core_message_take_short_value(message);
}

void *xt_core_message_take_double(xt_core_message_t *message)
{
  double *double_pointer;
  char *double_string;

  double_pointer = malloc(sizeof *double_pointer);
  if (double_pointer) {
    double_string = xt_core_message_take_string(message);
    if (double_string) {
      *double_pointer = atof(double_string);
      free(double_string);
    } else {
      *double_pointer = 0.0;
      xt_core_trace("x_core_message_take_string");
    }
  } else {
    xt_core_trace("malloc");
  }

  return double_pointer;
}

double xt_core_message_take_double_value(xt_core_message_t *message)
{
  double double_object;
  char *double_string;

  double_string = xt_core_message_take_string(message);
  if (double_string) {
    double_object = atof(double_string);
    free(double_string);
  } else {
    xt_core_trace("x_core_message_take_string");
    double_object = 0.0;
  }

  return double_object;
}

void *xt_core_message_take_long(xt_core_message_t *message)
{
  long *long_pointer;
  long long_object_network;

  memcpy(&long_object_network, message->data + message->deserialize_position,
      sizeof long_object_network);
  message->deserialize_position += sizeof long_object_network;
  long_pointer = malloc(sizeof *long_pointer);
  if (long_pointer) {
    *long_pointer = ntohl(long_object_network);
  } else {
    xt_core_trace("malloc");
  }

  return long_pointer;
}

long xt_core_message_take_long_value(xt_core_message_t *message)
{
  long long_object;
  long long_object_network;

  if ((message->deserialize_position + sizeof long_object_network)
      <= message->data_size) {
    memcpy(&long_object_network,
        message->data + message->deserialize_position,
        sizeof long_object_network);
    message->deserialize_position += sizeof long_object_network;
    long_object = ntohl(long_object_network);
  } else {
    xt_core_trace("tried to read a long past the end of the message");
    long_object = 0;
  }

  return long_object;
}

void *xt_core_message_take_short(xt_core_message_t *message)
{
  short *short_pointer;
  short short_object_network;

  memcpy(&short_object_network, message->data + message->deserialize_position,
      sizeof short_object_network);
  message->deserialize_position += sizeof short_object_network;
  short_pointer = malloc(sizeof *short_pointer);
  if (short_pointer) {
    *short_pointer = ntohs(short_object_network);
  } else {
    xt_core_trace("malloc");
  }

  return short_pointer;
}

short xt_core_message_take_short_value(xt_core_message_t *message)
{
  short short_object;
  short short_object_network;

  if ((message->deserialize_position + sizeof short_object_network)
      <= message->data_size) {
    memcpy(&short_object_network,
        message->data + message->deserialize_position,
        sizeof short_object_network);
    message->deserialize_position += sizeof short_object_network;
    short_object = ntohs(short_object_network);
  } else {
    xt_core_trace("tried to read a short past the end of the message");
    short_object = 0;
  }

  return short_object;
}

void *xt_core_message_take_string(xt_core_message_t *message)
{
  char *string;
  size_t string_size_with_terminator;

  string_size_with_terminator
    = strlen(message->data + message->deserialize_position) + 1;
  string = malloc(string_size_with_terminator);
  if (string) {
    memcpy(string, message->data + message->deserialize_position,
        string_size_with_terminator);
    message->deserialize_position += string_size_with_terminator;
  } else {
    xt_core_trace("malloc");
  }

  return string;
}
