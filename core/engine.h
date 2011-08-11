#ifndef xt_net_engine_h
#define xt_net_engine_h

#define XT_NET_ENGINE_NO_GET_NAME_FUNCTION NULL

#define XT_NET_ENGINE_TYPE_COUNT 5
enum xt_net_engine_id_t {
  XT_NET_ENGINE_UNKNOWN = 0,
  XT_NET_ENGINE_TEST = 1,
  XT_NET_ENGINE_PING = 2,
  XT_NET_ENGINE_SHELL = 3,
  XT_NET_ENGINE_HTTP = 4,
  XT_NET_ENGINE_MAX = 31,
};
typedef enum xt_net_engine_id_t xt_net_engine_id_t;

typedef char *(*xt_net_engine_get_name_f)(unsigned long engine_id);

char *xt_net_engine_get_name(unsigned long engine_id,
    xt_net_engine_get_name_f get_engine_name);

#endif
