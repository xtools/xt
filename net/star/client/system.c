#include "xt/case/set.h"
#include "xt/core/nameobject.h"
#include "xt/sync/period.h"
#include "xt/core/tools.h"
#include "xt/net/client/system.h"
#include "xt/net/star/client/system.h"
#include "xt/net/tools.h"

/*
  TODO: do we need to remove messages from the unsent queue if they've been
  there too long?
*/

#define DEFAULT_UNSENT_MESSAGES_QUEUE_SIZE 1024
#define MAINTAIN_PERIOD 8
#define MAX_CLIENTS 1024

struct message_handler_info_t {
  xt_net_engine_id_t engine_id;
  unsigned long message_type;
  xt_net_client_system_handle_message_f message_handler;
};
typedef struct message_handler_info_t message_handler_info_t;

struct xt_net_star_client_system_t {
  xt_case_list_t *star_arm_ips;
  unsigned short star_arm_port_min;
  unsigned short star_arm_port_max;

  char *node_server_exclude_ip;
  unsigned short node_server_exclude_min_port;
  unsigned short node_server_exclude_max_port;

  xt_case_set_t *clients;
  xt_case_list_t *client_list;

  xt_net_maintain_t maintain_schedule;
  xt_core_bool_t stop_requested;
  xt_sync_period_t *maintain_period;

  xt_case_list_t *unsent_messages;
  unsigned long unsent_messages_queue_size;

  unsigned long last_star_arm_count;
  xt_core_bool_t need_to_print_stats;

  long message_type_counts[XT_NET_ENGINE_TYPE_COUNT];
  xt_case_list_t *message_handler_info;

  void *custom_client_context;
  xt_core_log_t *log;

  xt_core_iobject_t nameobject_iobject;
};

static xt_core_bool_t client_connected(xt_net_star_client_system_t *starclient,
    char *client_name);

static char *create_client_name(char *server_ip, unsigned short server_port);

static void establisx_connection(xt_net_star_client_system_t *starclient,
    char *client_name, char *server_ip, unsigned short server_min_port,
    unsigned short server_max_port, void *custom_client_context);

static xt_core_bool_t exclude_ip_port_combination
(xt_net_star_client_system_t *starclient, char *ip, unsigned short port);

static xt_net_client_system_t *find_client(xt_net_star_client_system_t *starclient,
    char *client_name);

static xt_net_client_system_t *get_random_client(xt_net_star_client_system_t *starclient);

static xt_core_bool_t xt_net_star_client_system_create_client_list
(xt_net_star_client_system_t *starclient);

static xt_core_bool_t xt_net_star_client_system_create_clients
(xt_net_star_client_system_t *starclient);

static xt_core_bool_t xt_net_star_client_system_create_maintain_period
(xt_net_star_client_system_t *starclient);

static xt_core_bool_t xt_net_star_client_system_create_message_handler_info
(xt_net_star_client_system_t *starclient);

static xt_core_bool_t xt_net_star_client_system_create_unsent_messages
(xt_net_star_client_system_t *starclient);

static void xt_net_star_client_system_create_rollback(xt_net_star_client_system_t *starclient);

static void print_stats(xt_net_star_client_system_t *starclient);

static xt_core_bool_t put_messinferno_in_unsent_queue
(xt_net_star_client_system_t *starclient, xt_core_message_t *message);

static void re_route_unsent_messages(xt_net_star_client_system_t *starclient);

static void rebuild_client_list(xt_net_star_client_system_t *starclient);

static xt_core_bool_t register_engines_with_client
(xt_net_star_client_system_t *starclient, xt_net_client_system_t *client);

static void register_message_handlers_with_client
(xt_net_star_client_system_t *starclient, xt_net_client_system_t *client);

static void take_unsent_messages(xt_net_star_client_system_t *starclient,
    xt_net_client_system_t *client);

xt_core_bool_t client_connected(xt_net_star_client_system_t *starclient,
    char *client_name)
{
  assert(starclient);
  assert(client_name);
  xt_core_bool_t connected;
  xt_net_client_system_t *client;
  xt_core_nameobject_t *nameclient_decoy;

  client = find_client(starclient, client_name);
  if (client) {
    if (xt_net_client_system_is_connected_to_server(client)) {
      connected = xt_core_bool_true;
    } else {
      connected = xt_core_bool_false;
      take_unsent_messages(starclient, client);
      nameclient_decoy = xt_core_nameobject_create_decoy(client_name);
      if (nameclient_decoy) {
        if (xt_case_set_remove(starclient->clients, nameclient_decoy)) {
          printf("[star] lost connection with %s\n", client_name);
          starclient->need_to_print_stats = xt_core_bool_true;
        } else {
          xt_core_trace("this should never happen");
        }
        xt_core_nameobject_destroy_decoy(nameclient_decoy);
      } else {
        xt_core_trace("x_core_nameobject_create_decoy");
      }
    }
  } else {
    connected = xt_core_bool_false;
  }

  return connected;
}

char *create_client_name(char *server_ip, unsigned short server_port)
{
  assert(server_ip);
  char *client_name;
  unsigned short server_ip_size;
  unsigned short server_port_string_size;
  unsigned short client_name_size;
  char *server_port_string;

  client_name = NULL;

  server_ip_size = strlen(server_ip);
  server_port_string = malloc(6);
  if (server_port_string) {
    if (snprintf(server_port_string, 6, "%i", server_port) >= 0) {
      server_port_string_size = strlen(server_port_string);
      client_name_size = server_ip_size + 1 + server_port_string_size;
      client_name = malloc(client_name_size + 1);
      if (client_name) {
        memcpy(client_name, server_ip, server_ip_size);
        memcpy(client_name + server_ip_size, ":", 1);
        memcpy(client_name + server_ip_size + 1, server_port_string,
            server_port_string_size);
        *(client_name + client_name_size) = '\0';
      } else {
        xt_core_trace("malloc");
      }
    } else {
      xt_core_trace("snprintf");
    }
  } else {
    xt_core_trace("malloc");
  }
  free(server_port_string);

  return client_name;
}

void establisx_connection(xt_net_star_client_system_t *starclient, char *client_name,
    char *server_ip, unsigned short server_min_port,
    unsigned short server_max_port, void *custom_client_context)
{
  assert(starclient);
  assert(client_name);
  xt_core_bool_t established_connection;
  xt_net_client_system_t *client;
  xt_core_nameobject_t *nameclient;

  established_connection = xt_core_bool_false;

  client = xt_net_client_system_create(server_ip, server_min_port, server_max_port,
      XT_NET_ENGINE_NO_GET_NAME_FUNCTION, custom_client_context,
      starclient->log);
  if (client) {
    nameclient = xt_core_nameobject_create(client_name, client,
        XT_CORE_OBJECT_NO_COPY_F, xt_net_client_system_destroy,
        xt_net_client_system_get_as_string);
    if (nameclient) {
      if (xt_case_set_add(starclient->clients, nameclient)) {
        if (register_engines_with_client(starclient, client)) {
          register_message_handlers_with_client(starclient, client);
          printf("[star] connect to %s\n", client_name);
          starclient->need_to_print_stats = xt_core_bool_true;
          established_connection = xt_core_bool_true;
        } else {
          xt_core_trace("register_engines_with_client");
        }
      } else {
        xt_core_trace("x_case_set_add");
        xt_core_nameobject_destroy(nameclient);
      }
    } else {
      xt_core_trace("x_core_nameobject_create");
    }
  }
  if (!established_connection) {
    xt_net_client_system_destroy(client);
  }
}

xt_core_bool_t exclude_ip_port_combination(xt_net_star_client_system_t *starclient,
    char *ip, unsigned short port)
{
  assert(starclient);
  assert(ip);
  assert(port);
  xt_core_bool_t exclude;

  if (((port >= starclient->node_server_exclude_min_port)
          && (port <= starclient->node_server_exclude_max_port))
      && (0 == strcmp(ip, starclient->node_server_exclude_ip))) {
    exclude = xt_core_bool_true;
  } else {
    exclude = xt_core_bool_false;
  }

  return exclude;
}

xt_net_client_system_t *find_client(xt_net_star_client_system_t *starclient, char *client_name)
{
  assert(starclient);
  assert(client_name);
  xt_core_nameobject_t *nameclient_decoy;
  xt_core_nameobject_t *nameclient;
  xt_net_client_system_t *client;

  nameclient_decoy = xt_core_nameobject_create_decoy(client_name);
  if (nameclient_decoy) {
    nameclient = xt_case_set_find(starclient->clients, nameclient_decoy);
    xt_core_nameobject_destroy_decoy(nameclient_decoy);
    if (nameclient) {
      client = xt_core_nameobject_get_object(nameclient);
    } else {
      client = NULL;
    }
  } else {
    xt_core_trace_exit("x_core_nameobject_create_decoy");
    client = NULL;
  }

  return client;
}

xt_net_client_system_t *get_random_client(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  xt_core_nameobject_t *nameclient;
  xt_net_client_system_t *client;

  nameclient = xt_case_list_find_random(starclient->client_list);
  if (nameclient) {
    client = xt_core_nameobject_get_object(nameclient);
  } else {
    client = NULL;
  }

  return client;
}

xt_core_bool_t xt_net_star_client_system_connect(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  xt_core_bool_t connected_to_at_least_one;
  char *arm_ip;
  unsigned short port;
  char *client_name;
  unsigned long star_arm_count;

  xt_case_list_iterate_start(starclient->star_arm_ips);
  while ((arm_ip = xt_case_list_iterate_next(starclient->star_arm_ips))) {
    for (port = starclient->star_arm_port_min;
         port <= starclient->star_arm_port_max; port++) {
      if (!exclude_ip_port_combination(starclient, arm_ip, port)) {
        client_name = create_client_name(arm_ip, port);
        if (!client_connected(starclient, client_name)) {
          establisx_connection(starclient, client_name, arm_ip, port, port,
              starclient->custom_client_context);
        }
        free(client_name);
        client_name = NULL;
      }
    }
  }

  star_arm_count = xt_case_set_get_size(starclient->clients);
  if (star_arm_count != starclient->last_star_arm_count) {
    starclient->need_to_print_stats = xt_core_bool_true;
  }

  if (xt_case_set_get_size(starclient->clients) > 0) {
    connected_to_at_least_one = xt_core_bool_true;
  } else {
    connected_to_at_least_one = xt_core_bool_false;
  }

  return connected_to_at_least_one;
}

xt_net_star_client_system_t *xt_net_star_client_system_create(xt_case_list_t *star_arm_ips,
    unsigned short star_arm_port_min, unsigned short star_arm_port_max,
    char *node_server_exclude_ip, unsigned short node_server_exclude_min_port,
    unsigned short node_server_exclude_max_port, void *custom_client_context,
    xt_core_log_t *log)
{
  assert(star_arm_ips);
  assert(log);
  xt_net_star_client_system_t *starclient;
  xt_core_bool_t so_far_so_good;
  unsigned long engine_id;

  starclient = malloc(sizeof *starclient);
  if (starclient) {
    starclient->log = log;
    starclient->stop_requested = xt_core_bool_false;
    starclient->last_star_arm_count = 0;
    starclient->need_to_print_stats = xt_core_bool_false;
    starclient->star_arm_ips = star_arm_ips;
    starclient->star_arm_port_min = star_arm_port_min;
    starclient->star_arm_port_max = star_arm_port_max;
    starclient->node_server_exclude_ip = node_server_exclude_ip;
    starclient->node_server_exclude_min_port = node_server_exclude_min_port;
    starclient->node_server_exclude_max_port = node_server_exclude_max_port;
    starclient->custom_client_context = custom_client_context;
    starclient->unsent_messages_queue_size
      = DEFAULT_UNSENT_MESSAGES_QUEUE_SIZE;
    xt_core_nameobject_init_iobject(&starclient->nameobject_iobject);
    for (engine_id = 0; engine_id < XT_NET_ENGINE_TYPE_COUNT; engine_id++) {
      *(starclient->message_type_counts + engine_id) = -1;
    }
    if (xt_net_star_client_system_create_clients(starclient)) {
      so_far_so_good = xt_core_bool_true;
    } else {
      so_far_so_good = xt_core_bool_false;
    }
  } else {
    xt_core_trace("malloc");
    so_far_so_good = xt_core_bool_false;
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_star_client_system_create_maintain_period(starclient);
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_star_client_system_create_unsent_messages(starclient);
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_star_client_system_create_client_list(starclient);
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_star_client_system_create_message_handler_info(starclient);
  }

  if (!so_far_so_good && starclient) {
    xt_net_star_client_system_create_rollback(starclient);
    starclient = NULL;
  }

  assert(!starclient
      || (
        starclient->clients && starclient->maintain_period
        && starclient->unsent_messages && starclient->client_list
        )
    );
  return starclient;
}

xt_core_bool_t xt_net_star_client_system_create_client_list
(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  xt_core_bool_t success;

  starclient->client_list = xt_case_list_create(xt_core_nameobject_compare,
      xt_core_nameobject_copy, XT_CORE_OBJECT_NO_DESTROY_F);
  if (starclient->client_list) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_net_star_client_system_create_clients(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  xt_core_bool_t success;

  starclient->clients
    = xt_case_set_create(&starclient->nameobject_iobject);
  if (starclient->clients) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_net_star_client_system_create_maintain_period
(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  xt_core_bool_t success;

  starclient->maintain_period = xt_sync_period_create(MAINTAIN_PERIOD);
  if (starclient->maintain_period) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
    xt_core_trace("new");
  }

  return success;
}

xt_core_bool_t xt_net_star_client_system_create_message_handler_info
(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  xt_core_bool_t success;

  starclient->message_handler_info = xt_case_list_create
    (XT_CORE_OBJECT_NO_COMPARE_F, XT_CORE_OBJECT_NO_COPY_F, free);
  if (starclient->message_handler_info) {
    success = xt_core_bool_true;
  } else {
    xt_core_trace("x_case_list_create");
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_net_star_client_system_create_unsent_messages
(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  xt_core_bool_t success;

  starclient->unsent_messages = xt_case_list_create
    (XT_CORE_OBJECT_NO_COMPARE_F, xt_core_message_copy,
        XT_CORE_OBJECT_NO_DESTROY_F);
  if (starclient->unsent_messages) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

void xt_net_star_client_system_create_rollback(xt_net_star_client_system_t *starclient)
{
  assert(starclient);

  if (starclient->clients) {
    xt_case_set_destroy(starclient->clients);
  }
  if (starclient->maintain_period) {
    xt_sync_period_destroy(starclient->maintain_period);
  }
  if (starclient->unsent_messages) {
    xt_case_list_destroy(starclient->unsent_messages);
  }
  if (starclient->client_list) {
    xt_case_list_destroy(starclient->client_list);
  }
  if (starclient->message_handler_info) {
    xt_case_list_destroy(starclient->message_handler_info);
  }
  free(starclient);
}

void xt_net_star_client_system_destroy(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  unsigned int unsent_message_count;

  unsent_message_count
    = xt_case_list_get_size(starclient->unsent_messages);

  if (unsent_message_count > 0) {
    printf("[star] %u messages in unsent queue were lost\n",
        unsent_message_count);
  }

  xt_case_set_destroy(starclient->clients);
  xt_sync_period_destroy(starclient->maintain_period);
  xt_case_list_destroy(starclient->unsent_messages);
  xt_case_list_destroy(starclient->client_list);
  xt_case_list_destroy(starclient->message_handler_info);
  free(starclient);
}

xt_net_client_system_t *xt_net_star_client_system_get_client(xt_net_star_client_system_t *starclient,
    int socket)
{
  assert(starclient);
  xt_core_nameobject_t *nameclient;
  xt_net_client_system_t *client;
  xt_core_bool_t found_it;
  int each_socket;

  found_it = xt_core_bool_false;
  client = NULL;

  xt_case_list_iterate_start(starclient->client_list);
  while (!found_it && (nameclient = xt_case_list_iterate_next
          (starclient->client_list))) {
    client = xt_core_nameobject_get_object(nameclient);
    each_socket = xt_net_client_system_get_socket(client);
    if (each_socket == socket) {
      found_it = xt_core_bool_true;
    }
  }

  return client;
}

void xt_net_star_client_system_get_stats(xt_net_star_client_system_t *starclient,
    xt_net_star_client_stats_t *starclient_stats)
{
  assert(starclient);
  assert(starclient_stats);

  starclient_stats->connected_server_count
    = xt_case_set_get_size(starclient->clients);
}

void xt_net_star_client_system_process_messages(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  xt_core_nameobject_t *nameclient;
  xt_net_client_system_t *client;

  xt_case_set_iterate_start(starclient->clients);
  while ((nameclient = xt_case_set_iterate_next(starclient->clients))) {
    client = xt_core_nameobject_get_object(nameclient);
    xt_net_client_system_process_messages(client);
  }

  if (xt_sync_period_once(starclient->maintain_period)) {
    xt_net_star_client_system_connect(starclient);
    rebuild_client_list(starclient);
    re_route_unsent_messages(starclient);
    if (starclient->need_to_print_stats) {
      print_stats(starclient);
    }
  }
}

xt_core_bool_t xt_net_star_client_system_register_engine(xt_net_star_client_system_t *starclient,
    xt_net_engine_id_t engine_id, unsigned long message_type_count)
{
  assert(starclient);
  xt_core_nameobject_t *nameclient;
  xt_net_client_system_t *client;
  xt_core_bool_t success;

  success = xt_core_bool_true;

  *(starclient->message_type_counts + engine_id) = message_type_count;

  xt_case_set_iterate_start(starclient->clients);
  while ((nameclient = xt_case_set_iterate_next(starclient->clients))) {
    client = xt_core_nameobject_get_object(nameclient);
    if (!xt_net_client_system_register_engine(client, engine_id, message_type_count)) {
      success = xt_core_bool_false;
    }
  }

  return success;
}

void xt_net_star_client_system_register_message_handler(xt_net_star_client_system_t *starclient,
    xt_net_engine_id_t engine_id, unsigned long message_type,
    xt_net_client_system_handle_message_f message_handler)
{
  assert(starclient);
  xt_core_nameobject_t *nameclient;
  xt_net_client_system_t *client;
  message_handler_info_t *message_handler_info;

  message_handler_info = malloc(sizeof *message_handler_info);
  if (message_handler_info) {
    message_handler_info->engine_id = engine_id;
    message_handler_info->message_type = message_type;
    message_handler_info->message_handler = message_handler;
    if (!xt_case_list_add_last
        (starclient->message_handler_info, message_handler_info)) {
      xt_core_trace("x_case_list_add_last");
      free(message_handler_info);
    }
  } else {
    xt_core_trace("malloc");
  }

  xt_case_set_iterate_start(starclient->clients);
  while ((nameclient = xt_case_set_iterate_next(starclient->clients))) {
    client = xt_core_nameobject_get_object(nameclient);
    xt_net_client_system_register_message_handler
      (client, engine_id, message_type, message_handler);
  }
}

xt_core_bool_t xt_net_star_client_system_send_message_to_any_arm
(xt_net_star_client_system_t *starclient, xt_core_message_t *message)
{
  assert(starclient);
  assert(message);
  xt_net_client_system_t *random_client;
  xt_core_bool_t success;
  xt_core_bool_t sent_to_client;

  random_client = get_random_client(starclient);
  if (random_client) {
    if (xt_net_client_system_send_message(random_client, message)) {
      sent_to_client = xt_core_bool_true;
    } else {
      sent_to_client = xt_core_bool_false;
    }
  } else {
    sent_to_client = xt_core_bool_false;
  }

  if (sent_to_client) {
    success = xt_core_bool_true;
  } else {
    if (put_messinferno_in_unsent_queue(starclient, message)) {
      success = xt_core_bool_true;
    } else {
      printf("[star] unsent message queue is full, unable to send message\n");
      success = xt_core_bool_false;
    }
  }

  return success;
}

xt_core_bool_t xt_net_star_client_system_send_message_to_all_arms
(xt_net_star_client_system_t *starclient, xt_core_message_t *message)
{
  assert(starclient);
  assert(message);
  xt_core_bool_t success;
  xt_core_nameobject_t *nameclient;
  xt_net_client_system_t *client;
  xt_core_message_t *message_copy;

  success = xt_core_bool_true;

  xt_case_set_iterate_start(starclient->clients);
  while ((nameclient = xt_case_set_iterate_next(starclient->clients))) {
    client = xt_core_nameobject_get_object(nameclient);
    message_copy = xt_core_message_copy(message);
    if (message_copy) {
      if (!xt_net_client_system_send_message(client, message_copy)) {
        success = xt_core_bool_false;
        xt_core_message_destroy(message_copy);
      }
    } else {
      success = xt_core_bool_false;
      xt_core_trace("x_core_message_copy");
    }
  }

  xt_core_message_destroy(message);

  return success;
}

void xt_net_star_client_system_set_unsent_messages_queue_size
(xt_net_star_client_system_t *starclient, unsigned long queue_size)
{
  starclient->unsent_messages_queue_size = queue_size;
}

xt_core_bool_t xt_net_star_client_system_star_available(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  xt_core_bool_t available;
  unsigned long client_count;

  client_count = xt_case_set_get_size(starclient->clients);

  if (client_count > 0) {
    available = xt_core_bool_true;
  } else {
    available = xt_core_bool_false;
  }

  return available;
}

void print_stats(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  unsigned long star_arm_count;

  star_arm_count = xt_case_set_get_size(starclient->clients);

  printf("[star] connected to %lu arms of the star\n", star_arm_count);
  starclient->last_star_arm_count = star_arm_count;
  starclient->need_to_print_stats = xt_core_bool_false;
}

xt_core_bool_t put_messinferno_in_unsent_queue(xt_net_star_client_system_t *starclient,
    xt_core_message_t *message)
{
  assert(starclient);
  assert(message);
  unsigned long messages_in_queue;
  xt_core_bool_t success;

  messages_in_queue = xt_case_list_get_size(starclient->unsent_messages);

  if (starclient->unsent_messages_queue_size > messages_in_queue) {
    if (xt_case_list_add_last(starclient->unsent_messages, message)) {
      success = xt_core_bool_true;
    } else {
      success = xt_core_bool_false;
    }
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

void re_route_unsent_messages(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  xt_core_message_t *message;
  xt_case_list_t *unsent_messages;

  unsent_messages = starclient->unsent_messages;

  xt_case_list_iterate_start(unsent_messages);
  while ((message = xt_case_list_iterate_next(unsent_messages))) {
    xt_net_star_client_system_send_message_to_any_arm(starclient, message);
    xt_case_list_iterate_remove(unsent_messages);
  }
}

void rebuild_client_list(xt_net_star_client_system_t *starclient)
{
  assert(starclient);
  xt_core_nameobject_t *nameclient;

  xt_case_list_clear(starclient->client_list);
  xt_case_set_iterate_start(starclient->clients);
  while ((nameclient = xt_case_set_iterate_next(starclient->clients))) {
    xt_case_list_add_last(starclient->client_list, nameclient);
  }
}

xt_core_bool_t register_engines_with_client(xt_net_star_client_system_t *starclient,
    xt_net_client_system_t *client)
{
  assert(starclient);
  assert(client);
  xt_core_bool_t success;
  unsigned long engine_id;
  long message_type_count_long;
  unsigned long message_type_count;

  success = xt_core_bool_true;

  for (engine_id = 0; engine_id < XT_NET_ENGINE_TYPE_COUNT; engine_id++) {
    message_type_count_long = *(starclient->message_type_counts + engine_id);
    if (-1 != message_type_count_long) {
      message_type_count = (unsigned long) message_type_count_long;
      if (!xt_net_client_system_register_engine
          (client, engine_id, message_type_count)) {
        xt_core_trace("x_net_client_register_engine");
        success = xt_core_bool_false;
      }
    }
  }

  return success;
}

void register_message_handlers_with_client
(xt_net_star_client_system_t *starclient, xt_net_client_system_t *client)
{
  assert(starclient);
  assert(client);
  message_handler_info_t *message_handler_info;

  xt_case_list_iterate_start(starclient->message_handler_info);
  while ((message_handler_info
          = xt_case_list_iterate_next(starclient->message_handler_info))) {
    xt_net_client_system_register_message_handler(client,
        message_handler_info->engine_id, message_handler_info->message_type,
        message_handler_info->message_handler);
  }
}

void take_unsent_messages(xt_net_star_client_system_t *starclient,
    xt_net_client_system_t *client)
{
  assert(starclient);
  assert(client);
  xt_case_list_t *unsent_messages;
  xt_core_message_t *message;
  unsigned long discarded_message_count;

  discarded_message_count = 0;

  unsent_messages = xt_net_client_system_take_unsent_messages(client);
  xt_case_list_iterate_start(unsent_messages);
  while ((message = xt_case_list_iterate_next(unsent_messages))) {
    if (!put_messinferno_in_unsent_queue(starclient, message)) {
      discarded_message_count++;
      xt_core_message_destroy(message);
    }
  }
  xt_case_list_destroy(unsent_messages);

  if (discarded_message_count > 0) {
    printf("[star] unsent message queue is full, unable to take %lu "
        "unsent messages from client\n", discarded_message_count);
  }
}
