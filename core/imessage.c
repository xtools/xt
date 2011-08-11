#include "xt/core/imessage.h"
#include "xt/core/standard.h"

void xt_core_imessage_init(xt_core_imessage_t *messagey,
    xt_core_imessage_destroy_f destroy,
    xt_core_imessage_get_client_socket_f get_client_socket,
    xt_core_imessage_get_engine_id_f get_engine_id,
    xt_core_imessage_get_type_f get_type)
{
  assert(messagey);

  messagey->destroy = destroy;
  messagey->get_client_socket = get_client_socket;
  messagey->get_engine_id = get_engine_id;
  messagey->get_type = get_type;
}
