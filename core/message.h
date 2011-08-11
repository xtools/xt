#ifndef xt_core_message_h
#define xt_core_message_h

#include "xt/core/engine.h"
#include "xt/core/object.h"

enum xt_core_message_encoding_t {
  XT_CORE_MESSAGE_ENCODING_UNKNOWN = 0,
  XT_CORE_MESSAGE_ENCODING_PLAIN = 1,
};
typedef enum xt_core_message_encoding_t xt_core_message_encoding_t;

typedef xt_core_bool_t xt_core_message_add_to_message_f(void *object,
    xt_core_message_t *message);

typedef void *xt_core_message_create_from_message_f(xt_core_message_t *message);

xt_core_bool_t xt_core_message_add_bool(xt_core_message_t *message, void *object);

xt_core_bool_t xt_core_message_add_double(xt_core_message_t *message,
    void *object);

xt_core_bool_t xt_core_message_add_long(xt_core_message_t *message, void *object);

xt_core_bool_t xt_core_message_add_short(xt_core_message_t *message,
    void *object);

xt_core_bool_t xt_core_message_add_string(xt_core_message_t *message,
    void *object);

void *xt_core_message_copy(void *message_object);

xt_core_message_t *xt_core_message_create(int client_socket,
    xt_net_engine_id_t engine_id, unsigned long message_type, char *data,
    unsigned long data_size);

void xt_core_message_destroy(void *message_object);

int xt_core_message_get_client_socket(void *message_object);

char *xt_core_message_get_data(xt_core_message_t *message);

unsigned long xt_core_message_get_data_get_size(xt_core_message_t *message);

xt_core_message_encoding_t xt_core_message_get_encoding
(xt_core_message_t *message);

xt_net_engine_id_t xt_core_message_get_engine_id(void *message_object);

unsigned long xt_core_message_get_type(void *message_object);

void *xt_core_message_take_bool(xt_core_message_t *message);

xt_core_bool_t xt_core_message_take_bool_value(xt_core_message_t *message);

void *xt_core_message_take_double(xt_core_message_t *message);

double xt_core_message_take_double_value(xt_core_message_t *message);

void *xt_core_message_take_long(xt_core_message_t *message);

long xt_core_message_take_long_value(xt_core_message_t *message);

void *xt_core_message_take_short(xt_core_message_t *message);

short xt_core_message_take_short_value(xt_core_message_t *message);

void *xt_core_message_take_string(xt_core_message_t *message);

#endif
