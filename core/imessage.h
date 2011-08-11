#ifndef xt_core_imessage_h
#define xt_core_imessage_h

#include "xt/core/engine.h"
#include "xt/core/object.h"

#define XT_CORE_IMESSAGE_NO_CLIENT_SOCKET -1
#define XT_CORE_IMESSAGE_NO_TYPE 0
#define XT_CORE_IMESSAGE_NULL_DATA NULL
#define XT_CORE_IMESSAGE_ZERO_DATA_SIZE 0

typedef void (*xt_core_imessage_destroy_f)(void *message_object);

typedef int (*xt_core_imessage_get_client_socket_f)(void *message_object);

typedef xt_net_engine_id_t (*xt_core_imessage_get_engine_id_f)
(void *message_object);

typedef unsigned long (*xt_core_imessage_get_type_f)(void *message_object);

struct xt_core_imessage_t {
  xt_core_imessage_destroy_f destroy;
  xt_core_imessage_get_client_socket_f get_client_socket;
  xt_core_imessage_get_engine_id_f get_engine_id;
  xt_core_imessage_get_type_f get_type;
};
typedef struct xt_core_imessage_t xt_core_imessage_t;

void xt_core_imessage_init(xt_core_imessage_t *messagey,
    xt_core_imessage_destroy_f destroy,
    xt_core_imessage_get_client_socket_f get_client_socket,
    xt_core_imessage_get_engine_id_f get_engine_id,
    xt_core_imessage_get_type_f get_type);

#endif
