#include "xt/core/standard.h"
#include "xt/net/engine/iengine.h"

void xt_net_engine_iengine_init(xt_net_engine_iengine_t *iengine,
    xt_net_engine_iengine_create_f create,
    xt_net_engine_iengine_destroy_f destroy,
    xt_net_engine_iengine_get_handler_for_message_f get_handler_for_message,
    xt_net_engine_iengine_maintain_f maintain,
    xt_net_engine_iengine_run_f run, xt_net_engine_iengine_start_f start,
    xt_net_engine_iengine_stop_f stop)
{
  assert(iengine);

  iengine->create = create;
  iengine->destroy = destroy;
  iengine->get_handler_for_message = get_handler_for_message;
  iengine->maintain = maintain;
  iengine->run = run;
  iengine->start = start;
  iengine->stop = stop;
}
