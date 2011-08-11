#include "xt/case/list.h"
#include "xt/case/set.h"
#include "xt/core/basic/short.h"
#include "xt/core/engine.h"
#include "xt/core/message.h"
#include "xt/core/time.h"
#include "xt/core/tools.h"
#include "xt/net/client/socket.h"
#include "xt/net/engine/thread.h"
#include "xt/net/engine/iengine.h"
#include "xt/net/exchange.h"
#include "xt/net/post/system.h"
#include "xt/net/server/system.h"
#include "xt/net/server/socket.h"
#include "xt/sync/period.h"

#define DEFAULT_UNRESPONSIVE_CLIENT_TIME_SECONDS 32
#define ENGINE_INITIAL_SLEEP_MICROSECONDS XT_CORE_STANDARD_SLEEP_MICROSECONDS
#define XT_NET_SERVER_SYSTEM_SLEEP_MICROSECONDS \
  XT_CORE_STANDARD_SLEEP_MICROSECONDS
#define MAX_THREADS_PER_ENGINE 8
#define PERFORMANCE_PERIOD_SECONDS 4

struct engine_container_t {
  xt_net_engine_id_t engine_id;
  xt_net_engine_iengine_t *iengine;

  void *engine_object;
  xt_core_bool_t stop_requested;
  unsigned long sleep;

  xt_case_list_t *inbox;

  pthread_t threads[MAX_THREADS_PER_ENGINE];
  xt_core_bool_t run_threads_quiesce_requested[MAX_THREADS_PER_ENGINE];
  xt_core_bool_t run_threads_quiesce_completed[MAX_THREADS_PER_ENGINE];
  xt_case_list_t *thread_inboxes[MAX_THREADS_PER_ENGINE];
  pthread_mutex_t thread_inbox_mutexes[MAX_THREADS_PER_ENGINE];

  unsigned short min_run_threads;
  unsigned short max_run_threads;
  unsigned short run_thread_count;

  pthread_t maintain_thread;
  xt_net_maintain_t maintain_schedule;

  xt_net_server_system_handle_message_f *message_handlers;
  unsigned long message_handlers_size;

  double messages_per_second;
  double messages_per_second_thread[MAX_THREADS_PER_ENGINE];
  xt_sync_period_t *performance_period;
  unsigned long messages_per_period;
  xt_core_bool_t reported_idle;
  unsigned long messages_per_period_thread[MAX_THREADS_PER_ENGINE];
};
typedef struct engine_container_t engine_container_t;

struct xt_net_server_system_t {
  char *name;
  int socket;
  unsigned short min_port;
  unsigned short max_port;

  unsigned short max_threads;
  unsigned short thread_count;

  xt_net_server_stats_t stats;

  pthread_t server_thread;
  pthread_t accept_thread;
  xt_core_bool_t accept_thread_created;

  xt_case_list_t *outbox;
  pthread_mutex_t outbox_mutex;

  xt_case_set_t *client_posts;
  xt_net_exchange_t *client_exchange;
  pthread_mutex_t client_posts_mutex;

  xt_case_list_t *engines;
  engine_container_t *engines_array[XT_NET_SERVER_SYSTEM_MAX_ENGINES];

  void *custom_server_object;

  unsigned long unresponsive_client_time_seconds;

  xt_core_imessage_t *messagey;
  xt_net_post_ipost_t *ipost;
  xt_net_engine_get_name_f get_engine_name;

  time_t create_time;

  xt_core_log_t *log;

  xt_config_system_t *config_system;

  xt_core_iobject_t client_posts_iobject;
};

struct running_engine_t {
  xt_net_server_system_t *server;
  void *engine_object;
  xt_net_engine_id_t engine_id;

  xt_net_engine_iengine_start_f start;
  xt_net_engine_iengine_run_f run;
  xt_net_engine_iengine_stop_f stop;

  unsigned short thread_index;
};
typedef struct running_engine_t running_engine_t;

struct maintaining_engine_t {
  xt_net_server_system_t *server;
  void *engine_object;
  xt_net_engine_id_t engine_id;

  xt_net_engine_iengine_maintain_f maintain;
};
typedef struct maintaining_engine_t maintaining_engine_t;

static void *accept_thread(void *server_object);

static void close_disconnected_clients(xt_net_server_system_t *server);

static void close_unresponsive_clients(xt_net_server_system_t *server);

static engine_container_t *create_engine_container(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id, void *custom_server_object,
    xt_net_engine_iengine_t *iengine, unsigned short min_run_threads,
    unsigned short max_run_threads, xt_net_maintain_t maintain_schedule,
    unsigned long message_type_count);

static xt_core_bool_t create_engine_container_message_handlers
(engine_container_t *engine_container, unsigned long message_type_count,
    xt_core_log_t *log);

static xt_core_bool_t create_engine_container_performance_period
(engine_container_t *engine_container, xt_core_log_t *log);

static void create_engine_container_rollback
(engine_container_t *engine_container);

static void create_engine_container_stats
(engine_container_t *engine_container);

static xt_core_bool_t create_engine_container_threads
(engine_container_t *engine_container, xt_core_imessage_t *messagey,
    xt_core_log_t *log);

static maintaining_engine_t *create_maintaining_engine
(xt_net_server_system_t *server, void *engine_object,
    xt_net_engine_id_t engine_id,
    xt_net_engine_iengine_maintain_f maintain);

static xt_core_bool_t create_post_for_new_client
(xt_net_server_system_t *server, int client_socket);

static running_engine_t *create_running_engine
(xt_net_server_system_t *server, void *engine_object,
    xt_net_engine_id_t engine_id, xt_net_engine_iengine_start_f start,
    xt_net_engine_iengine_run_f run, xt_net_engine_iengine_stop_f stop,
    unsigned short thread_index);

static xt_core_bool_t create_thread(xt_net_server_system_t *server,
    pthread_t *pthread_address, xt_core_thread_f thread_function,
    void *thread);

static void deliver_messages_to_engine(xt_net_server_system_t *server,
    engine_container_t *engine_container);

static unsigned long deliver_messages_to_engine_thread
(xt_net_server_system_t *server, engine_container_t *engine_container,
    unsigned short thread_index,
    unsigned long target_number_of_messages_to_deliver);

static void deliver_messages_to_engines(xt_net_server_system_t *server);

static void destroy_engine_container(void *engine_container_object);

static void destroy_maintaining_engine
(maintaining_engine_t *maintaining_engine);

static void destroy_running_engine(running_engine_t *running_engine);

static unsigned long engine_count(xt_net_server_system_t *server);

static xt_core_bool_t engine_is_registered(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id);

static void engine_sleep(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id);

static void *find_client_post(xt_net_server_system_t *server,
    int client_socket);

static void handle_signal(int signal);

static xt_core_bool_t xt_net_server_system_create_client_exchange
(xt_net_server_system_t *server);

static xt_core_bool_t xt_net_server_system_create_client_posts
(xt_net_server_system_t *server);

static xt_core_bool_t xt_net_server_system_create_engines
(xt_net_server_system_t *server);

static xt_core_bool_t xt_net_server_system_create_outbox
(xt_net_server_system_t *server);

static void xt_net_server_system_create_rollback
(xt_net_server_system_t *server);

static xt_core_bool_t xt_net_server_system_register_engine_create
(engine_container_t *engine_container, xt_net_engine_iengine_create_f create,
    xt_net_server_system_t *server, void *custom_server_object);

static xt_core_bool_t join_thread(xt_net_server_system_t *server,
    pthread_t thread);

static void *maintain_engine(void *maintaining_engine_object);

static void manage_threads(xt_net_server_system_t *server);

static void post_messages_to_clients(xt_net_server_system_t *server);

static void receive_messages_from_client_post(xt_net_server_system_t *server,
    void *post_object);

static void receive_messages_from_clients(xt_net_server_system_t *server);

static void *run_engine(void *running_engine_object);

static xt_core_bool_t serversocket_bind_listen
(xt_net_server_system_t *server);

static void *server_thread(void *server_object);

static void stop(xt_net_server_system_t *server);

static xt_core_bool_t start(xt_net_server_system_t *server);

static xt_core_bool_t start_engine(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id);

static xt_core_bool_t start_engine_maintain_thread
(xt_net_server_system_t *server, xt_net_engine_id_t engine_id,
    engine_container_t *engine_container);

static xt_core_bool_t start_engine_run_thread
(xt_net_server_system_t *server, xt_net_engine_id_t engine_id,
    engine_container_t *engine_container, unsigned short thread_index);

static xt_core_bool_t threads_maxed_out(xt_net_server_system_t *server);

void *accept_thread(void *server_object)
{
  xt_net_server_system_t *server;
  struct sockaddr_in client_address;
  socklen_t client_address_size;
  int client_socket;

  server = server_object;

  while (!xt_core_stop_requested) {
    client_socket = xt_net_server_socket_accept
      (server->socket, &client_address, &client_address_size);
    if (client_socket >= 0) {
      if (!create_post_for_new_client(server, client_socket)) {
        xt_core_log_trace
          (server->log, "hnet", "cx_net_server_socket_accept");
      }
    }
    usleep(XT_NET_SERVER_SYSTEM_SLEEP_MICROSECONDS);
  }

  return NULL;
}

void close_disconnected_clients(xt_net_server_system_t *server)
{
  assert(xt_case_set_get_size(server->client_posts)
      == xt_net_exchange_get_post_count(server->client_exchange));

  void *client_post_object;
  xt_case_set_t *client_posts;
  int client_socket;
  /*  xt_core_bool_t success;  */

  client_posts = server->client_posts;

  xt_case_set_iterate_start(client_posts);
  while ((client_post_object = xt_case_set_iterate_next(client_posts))) {
    if (server->ipost->socket_closed(client_post_object)) {
      client_socket = server->ipost->get_socket(client_post_object);
      xt_case_set_iterate_remove(client_posts);
      /*  success = xt_core_bool_true;  */
      if (!xt_net_exchange_unregister_post
          (server->client_exchange, client_socket)) {
        xt_core_log_trace
          (server->log, "hnet", "x_net_exchange_unregister_post");
        /*  success = xt_core_bool_false;  */
      }
      xt_net_client_socket_destroy(client_socket);
    }
  }

  assert(xt_case_set_get_size(server->client_posts)
      == xt_net_exchange_get_post_count(server->client_exchange));
}

void close_unresponsive_clients(xt_net_server_system_t *server)
{
  assert(xt_case_set_get_size(server->client_posts)
      == xt_net_exchange_get_post_count(server->client_exchange));
  void *client_post_object;
  xt_case_set_t *client_posts;
  int client_socket;
  time_t current_time;
  time_t post_activity_time;

  client_posts = server->client_posts;
  current_time = time(NULL);

  xt_case_set_iterate_start(client_posts);
  while ((client_post_object = xt_case_set_iterate_next(client_posts))) {
    post_activity_time = server->ipost->get_last_receive_activity_time
      (client_post_object);
    if (current_time - post_activity_time
        > server->unresponsive_client_time_seconds) {
      xt_case_set_iterate_remove(client_posts);
      client_socket = server->ipost->get_socket(client_post_object);
      if (!xt_net_exchange_unregister_post(server->client_exchange,
              client_socket)) {
        xt_core_log_trace
          (server->log, "hnet", "x_net_exchange_unregister_post");
      }
      xt_net_client_socket_destroy(client_socket);
      xt_core_log_enter(server->log, "hnet",
          "server closed unresponsive client %u", client_socket);
    }
  }
}

engine_container_t *create_engine_container(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id, void *custom_server_object,
    xt_net_engine_iengine_t *iengine, unsigned short min_run_threads,
    unsigned short max_run_threads, xt_net_maintain_t maintain_schedule,
    unsigned long message_type_count)
{
  assert(server);
  engine_container_t *engine_container;
  xt_core_bool_t so_far_so_good;

  engine_container = malloc(sizeof *engine_container);
  if (engine_container) {
    engine_container->engine_id = engine_id;
    engine_container->iengine = iengine;
    engine_container->stop_requested = xt_core_bool_false;
    engine_container->sleep = ENGINE_INITIAL_SLEEP_MICROSECONDS;
    engine_container->maintain_schedule = maintain_schedule;
    engine_container->min_run_threads = min_run_threads;
    engine_container->max_run_threads = max_run_threads;
    engine_container->run_thread_count = 0;
    create_engine_container_stats(engine_container);
    so_far_so_good = xt_core_bool_true;
  } else {
    so_far_so_good = xt_core_bool_false;
  }

  if (so_far_so_good) {
    engine_container->inbox = xt_case_list_create
      (XT_CORE_OBJECT_NO_COMPARE_F, XT_CORE_OBJECT_NO_COPY_F,
          XT_CORE_OBJECT_NO_DESTROY_F);
    if (!engine_container->inbox) {
      xt_core_log_trace(server->log, "hnet", "x_case_list_create");
      so_far_so_good = xt_core_bool_false;
    }
  }

  if (so_far_so_good) {
    so_far_so_good = create_engine_container_threads(engine_container,
        server->messagey,
        server->log);
  }

  if (so_far_so_good) {
    so_far_so_good = create_engine_container_message_handlers
      (engine_container, message_type_count, server->log);
  }

  if (so_far_so_good) {
    so_far_so_good = create_engine_container_performance_period
      (engine_container, server->log);
  }

  if (!so_far_so_good) {
    create_engine_container_rollback(engine_container);
    engine_container = NULL;
  }

  return engine_container;
}

xt_core_bool_t create_engine_container_message_handlers
(engine_container_t *engine_container, unsigned long message_type_count,
    xt_core_log_t *log)
{
  assert(engine_container);
  assert(log);
  unsigned long each_handler;
  xt_core_bool_t success;

  engine_container->message_handlers_size = message_type_count;
  engine_container->message_handlers
    = malloc((sizeof *engine_container->message_handlers)
        * message_type_count);
  if (engine_container->message_handlers) {
    for (each_handler = 0; each_handler < message_type_count; each_handler++) {
      *(engine_container->message_handlers + each_handler) = NULL;
    }
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
    xt_core_log_trace(log, "hnet", "malloc");
  }

  return success;
}

xt_core_bool_t create_engine_container_performance_period
(engine_container_t *engine_container, xt_core_log_t *log)
{
  assert(engine_container);
  assert(log);
  xt_core_bool_t success;

  engine_container->performance_period
    = xt_sync_period_create(PERFORMANCE_PERIOD_SECONDS);
  if (engine_container->performance_period) {
    success = xt_core_bool_true;
  } else {
    xt_core_log_trace(log, "hnet", "x_sync_period_create");
    success = xt_core_bool_false;
  }

  return success;
}

void create_engine_container_rollback(engine_container_t *engine_container)
{
  assert(engine_container);
  unsigned short each_thread;

  if (engine_container->engine_object) {
    engine_container->iengine->destroy(engine_container->engine_object);
  }
  if (engine_container->inbox) {
    xt_case_list_destroy(engine_container->inbox);
  }
  for (each_thread = 0; each_thread < MAX_THREADS_PER_ENGINE; each_thread++) {
    if (engine_container->thread_inboxes[each_thread]) {
      xt_case_list_destroy(engine_container->thread_inboxes[each_thread]);
    }
  }
  if (engine_container->message_handlers) {
    free(engine_container->message_handlers);
  }
  if (engine_container->performance_period) {
    xt_sync_period_destroy(engine_container->performance_period);
  }
  free(engine_container);
}

void create_engine_container_stats(engine_container_t *engine_container)
{
  assert(engine_container);
  unsigned short each_thread;

  engine_container->messages_per_second = 0.0;
  engine_container->messages_per_period = 0;
  engine_container->reported_idle = xt_core_bool_false;
  for (each_thread = 0; each_thread < MAX_THREADS_PER_ENGINE; each_thread++) {
    *(engine_container->messages_per_second_thread + each_thread) = 0.0;
    *(engine_container->messages_per_period_thread + each_thread) = 0;
  }
}

xt_core_bool_t create_engine_container_threads
(engine_container_t *engine_container, xt_core_imessage_t *messagey,
    xt_core_log_t *log)
{
  assert(engine_container);
  assert(messagey);
  assert(log);
  unsigned short each_thread;
  pthread_mutex_t *each_mutex;
  xt_core_bool_t success;

  success = xt_core_bool_true;

  for (each_thread = 0; each_thread < MAX_THREADS_PER_ENGINE; each_thread++) {

    *(engine_container->run_threads_quiesce_requested
        + each_thread) = xt_core_bool_false;
    *(engine_container->run_threads_quiesce_completed
        + each_thread) = xt_core_bool_false;

    *(engine_container->thread_inboxes + each_thread)
      = xt_case_list_create(XT_CORE_OBJECT_NO_COMPARE_F,
          XT_CORE_OBJECT_NO_COPY_F, messagey->destroy);
    if (!*(engine_container->thread_inboxes + each_thread)) {
      success = xt_core_bool_false;
      xt_core_log_trace(log, "hnet", "x_case_list_create");
      break;
    }

    each_mutex = &engine_container->thread_inbox_mutexes[each_thread];
    if (0 != pthread_mutex_init(each_mutex, NULL)) {
      success = xt_core_bool_false;
      xt_core_log_trace(log, "hnet", "pthread_mutex_init");
      break;
    }
  }

  return success;
}

maintaining_engine_t *create_maintaining_engine
(xt_net_server_system_t *server, void *engine_object,
    xt_net_engine_id_t engine_id,
    xt_net_engine_iengine_maintain_f maintain)
{
  maintaining_engine_t *maintaining_engine;

  maintaining_engine = malloc(sizeof *maintaining_engine);
  if (maintaining_engine) {
    maintaining_engine->server = server;
    maintaining_engine->engine_object = engine_object;
    maintaining_engine->engine_id = engine_id;
    maintaining_engine->maintain = maintain;
  } else {
    xt_core_log_trace(server->log, "hnet", "malloc");
  }

  return maintaining_engine;
}

xt_core_bool_t create_post_for_new_client(xt_net_server_system_t *server,
    int client_socket)
{
  assert(server);
  xt_core_bool_t success;
  void *post_object;

  post_object = server->ipost->create(client_socket);

  if (post_object) {

    pthread_mutex_lock(&server->client_posts_mutex);
    {
      if (xt_case_set_add(server->client_posts, post_object)) {
        success = xt_core_bool_true;
        if (!xt_net_exchange_register_post
            (server->client_exchange, post_object)) {
          xt_core_log_trace
            (server->log, "hnet", "x_net_exchange_register_post");
          success = xt_core_bool_false;
        }
      } else {
        xt_core_log_trace(server->log, "hnet", "x_case_set_add");
        success = xt_core_bool_false;
      }
    }
    pthread_mutex_unlock(&server->client_posts_mutex);

    if (!success) {
      server->ipost->destroy(post_object);
    }

  } else {
    xt_core_log_trace(server->log, "hnet", "create");
    success = xt_core_bool_false;
  }

  return success;
}

running_engine_t *create_running_engine(xt_net_server_system_t *server,
    void *engine_object, xt_net_engine_id_t engine_id,
    xt_net_engine_iengine_start_f start, xt_net_engine_iengine_run_f run,
    xt_net_engine_iengine_stop_f stop, unsigned short thread_index)
{
  running_engine_t *running_engine;

  running_engine = malloc(sizeof *running_engine);
  if (running_engine) {
    running_engine->server = server;
    running_engine->engine_object = engine_object;
    running_engine->engine_id = engine_id;
    running_engine->start = start;
    running_engine->run = run;
    running_engine->stop = stop;
    running_engine->thread_index = thread_index;
  }

  return running_engine;
}

xt_core_bool_t create_thread(xt_net_server_system_t *server,
    pthread_t *pthread_address, xt_core_thread_f thread_function,
    void *thread)
{
  xt_core_bool_t success;

  success = xt_core_bool_false;

  if (!threads_maxed_out(server)) {
    if (0 == pthread_create(pthread_address, NULL, thread_function, thread)) {
      success = xt_core_bool_true;
      server->thread_count++;
    } else {
      xt_core_log_trace(server->log, "hnet", "pthread_create");
    }
  } else {
    xt_core_log_enter(server->log, "hnet", "threads_maxed_out");
  }

  return success;
}

/*
  TODO: compartmentalize the reporting part of this function
*/
void deliver_messages_to_engine(xt_net_server_system_t *server,
    engine_container_t *engine_container)
{
  assert(server);
  assert(engine_container);
  unsigned short each_thread;
  unsigned long inbox_size;
  unsigned long target_number_of_messages_to_deliver;
  unsigned long actual_messages_delivered;
  unsigned long actual_messages_delivered_thread;
  double messages_per_second_this_period;
  double messages_per_second_this_period_on_this_thread;
  char *string;
  char little_string[5 + 1];

  actual_messages_delivered = 0;

  if (xt_sync_period_once(engine_container->performance_period)) {

    messages_per_second_this_period = engine_container->messages_per_period
      / ((double) PERFORMANCE_PERIOD_SECONDS);

    if (messages_per_second_this_period > 0.0) {
      engine_container->reported_idle = xt_core_bool_false;

      string = NULL;
      for (each_thread = 0; each_thread < engine_container->run_thread_count;
           each_thread++) {
        messages_per_second_this_period_on_this_thread
          = engine_container->messages_per_period_thread
          [each_thread] / ((double) PERFORMANCE_PERIOD_SECONDS);
        snprintf(little_string, 5 + 1, " %0.1f ",
            messages_per_second_this_period_on_this_thread);
        string = xt_core_string_append(string, little_string);
        if (!string) {
          xt_core_log_trace(server->log, "hnet", "x_core_string_append");
        }
      }
      xt_core_log_enter(server->log, "hnet", "%0.1f mps (%s) %s engine",
          messages_per_second_this_period, string,
          xt_net_engine_get_name(engine_container->engine_id,
              server->get_engine_name));
      free(string);

    } else if (!engine_container->reported_idle) {
      xt_core_log_enter(server->log, "hnet", "%s engine is idle",
          xt_net_engine_get_name(engine_container->engine_id,
              server->get_engine_name));
      engine_container->reported_idle = xt_core_bool_true;

    }

    engine_container->messages_per_period = 0;
    for (each_thread = 0; each_thread < engine_container->run_thread_count;
         each_thread++) {
      *(engine_container->messages_per_period_thread + each_thread) = 0;
    }
  }

  if (engine_container->run_thread_count > 0) {
    inbox_size = xt_case_list_get_size(engine_container->inbox);
    target_number_of_messages_to_deliver
      = inbox_size / engine_container->run_thread_count;
    if (0 == target_number_of_messages_to_deliver) {
      target_number_of_messages_to_deliver = 1;
    }

    for (each_thread = 0; each_thread < engine_container->run_thread_count;
         each_thread++) {
      actual_messages_delivered_thread = deliver_messages_to_engine_thread
        (server, engine_container, each_thread,
            target_number_of_messages_to_deliver);
      actual_messages_delivered += actual_messages_delivered_thread;
      engine_container->messages_per_period_thread[each_thread]
        += actual_messages_delivered_thread;
    }
    engine_container->messages_per_period += actual_messages_delivered;
  }
}

unsigned long deliver_messages_to_engine_thread
(xt_net_server_system_t *server, engine_container_t *engine_container,
    unsigned short thread_index,
    unsigned long target_number_of_messages_to_deliver)
{
  assert(server);
  assert(engine_container);
  unsigned long messages_delivered;
  unsigned long each_message;
  xt_case_list_t *thread_inbox;
  pthread_mutex_t *thread_inbox_mutex;
  void *message;

  messages_delivered = 0;
  thread_inbox_mutex = &engine_container->thread_inbox_mutexes[thread_index];
  thread_inbox = engine_container->thread_inboxes[thread_index];

  if (0 == pthread_mutex_trylock(thread_inbox_mutex)) {
    for (each_message = 0; each_message < target_number_of_messages_to_deliver;
         each_message++) {
      if ((message = xt_case_list_find_first(engine_container->inbox))) {
        if (xt_case_list_add_last(thread_inbox, message)) {
          xt_case_list_remove_first(engine_container->inbox);
          messages_delivered++;
        } else {
          xt_core_log_trace(server->log, "hnet", "x_case_list_add_last");
        }
      }
    }
    pthread_mutex_unlock(thread_inbox_mutex);
  }

  return messages_delivered;
}

void deliver_messages_to_engines(xt_net_server_system_t *server)
{
  assert(server);
  engine_container_t *engine_container;

  xt_case_list_iterate_start(server->engines);
  while ((engine_container = xt_case_list_iterate_next(server->engines))) {
    deliver_messages_to_engine(server, engine_container);
  }
}

void destroy_engine_container(void *engine_container_object)
{
  assert(engine_container_object);
  engine_container_t *engine_container;
  unsigned short each_thread;

  engine_container = engine_container_object;

  engine_container->iengine->destroy(engine_container->engine_object);
  for (each_thread = 0; each_thread < MAX_THREADS_PER_ENGINE; each_thread++) {
    xt_case_list_destroy(engine_container->thread_inboxes[each_thread]);
  }
  xt_case_list_destroy(engine_container->inbox);
  free(engine_container->message_handlers);
  xt_sync_period_destroy(engine_container->performance_period);

  free(engine_container);
}

void destroy_maintaining_engine(maintaining_engine_t *maintaining_engine)
{
  free(maintaining_engine);
}

void destroy_running_engine(running_engine_t *running_engine)
{
  free(running_engine);
}

unsigned long engine_count(xt_net_server_system_t *server)
{
  return xt_case_list_get_size(server->engines);
}

xt_core_bool_t engine_is_registered(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id)
{
  xt_core_bool_t is_registered;

  if (server->engines_array[engine_id]) {
    is_registered = xt_core_bool_true;
  } else {
    is_registered = xt_core_bool_false;
  }

  return is_registered;
}

void engine_sleep(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id)
{
  usleep(server->engines_array[engine_id]->sleep);
}

void *find_client_post(xt_net_server_system_t *server, int client_socket)
{
  void *client_post_object;
  void *decoy_post_object;

  decoy_post_object = server->ipost->create_decoy(client_socket);
  if (decoy_post_object) {
    client_post_object
      = xt_case_set_find(server->client_posts, decoy_post_object);
    server->ipost->destroy_decoy(decoy_post_object);
  } else {
    xt_core_log_trace(server->log, "hnet", "create_decoy");
    client_post_object = NULL;
  }

  return client_post_object;
}

void handle_signal(int signal)
{
  switch (signal) {
    case SIGUSR1:
      break;
    case SIGUSR2:
      break;
    default:
      xt_core_stop_requested = xt_core_bool_true;
      break;
  }
}

xt_net_server_system_t *xt_net_server_system_create(const char *name,
    unsigned short min_port, unsigned short max_port,
    unsigned short max_threads, xt_core_imessage_t *messagey,
    xt_net_post_ipost_t *ipost, xt_net_engine_get_name_f get_engine_name,
    xt_config_system_t *config_system, xt_core_log_t *log)
{
  assert(name);
  assert(messagey);
  assert(ipost);
  assert(log);
  xt_net_server_system_t *server;
  xt_core_bool_t so_far_so_good;

  server = malloc(sizeof *server);
  if (server) {
    server->min_port = min_port;
    server->max_port = max_port;
    server->max_threads = max_threads;
    server->thread_count = 0;
    server->unresponsive_client_time_seconds
      = DEFAULT_UNRESPONSIVE_CLIENT_TIME_SECONDS;
    xt_net_server_stats_init(&server->stats);
    server->custom_server_object = NULL;
    server->messagey = messagey;
    server->ipost = ipost;
    server->get_engine_name = get_engine_name;
    server->create_time = time(NULL);
    server->log = log;
    server->config_system = config_system;
    server->accept_thread_created = xt_core_bool_false;
    so_far_so_good = xt_core_bool_true;
  } else {
    so_far_so_good = xt_core_bool_false;
    xt_core_log_trace(server->log, "hnet", "malloc");
  }

  if (so_far_so_good) {
    server->name = strdup(name);
    if (!server->name) {
      so_far_so_good = xt_core_bool_false;
    }
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_server_system_create_outbox(server);
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_server_system_create_client_posts(server);
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_server_system_create_client_exchange(server);
  }

  if (so_far_so_good) {
    so_far_so_good = xt_net_server_system_create_engines(server);
  }

  if (!so_far_so_good) {
    xt_net_server_system_create_rollback(server);
    server = NULL;
  }

  return server;
}

xt_core_bool_t xt_net_server_system_create_client_exchange
(xt_net_server_system_t *server)
{
  assert(server);
  xt_core_bool_t success;

  server->client_exchange = xt_net_exchange_create(server->ipost);
  if (server->client_exchange) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
    xt_core_log_trace(server->log, "hnet", "x_net_exchange_create");
  }

  return success;
}

xt_core_bool_t xt_net_server_system_create_client_posts
(xt_net_server_system_t *server)
{
  assert(server);
  xt_core_bool_t success;

  xt_core_iobject_init(&server->client_posts_iobject, server->ipost->compare,
      server->ipost->compare_equal, XT_CORE_OBJECT_NO_COPY_F,
      server->ipost->destroy, XT_CORE_OBJECT_NO_GET_AS_STRING_F,
      server->ipost->mod);
  server->client_posts = xt_case_set_create(&server->client_posts_iobject);
  if (server->client_posts) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
    xt_core_log_trace(server->log, "hnet", "x_case_set_create");
  }

  if (success) {
    if (0 == pthread_mutex_init(&server->client_posts_mutex, NULL)) {
      success = xt_core_bool_true;
    } else {
      success = xt_core_bool_false;
      xt_core_log_trace(server->log, "hnet", "pthread_mutex_init");
      xt_case_set_destroy(server->client_posts);
    }
  }

  return success;
}

xt_core_bool_t xt_net_server_system_create_engines
(xt_net_server_system_t *server)
{
  assert(server);
  xt_core_bool_t success;
  unsigned long each_engine_id;

  server->engines = xt_case_list_create(XT_CORE_OBJECT_NO_COMPARE_F,
      XT_CORE_OBJECT_NO_COPY_F, destroy_engine_container);
  if (server->engines) {
    for (each_engine_id = 0;
         each_engine_id < XT_NET_SERVER_SYSTEM_MAX_ENGINES;
         each_engine_id++) {
      server->engines_array[each_engine_id] = NULL;
    }
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
    xt_core_log_trace(server->log, "hnet", "x_case_list_create");
  }

  return success;
}

xt_core_bool_t xt_net_server_system_create_outbox
(xt_net_server_system_t *server)
{
  assert(server);
  xt_core_bool_t success;

  server->outbox = xt_case_list_create(XT_CORE_OBJECT_NO_COMPARE_F,
      XT_CORE_OBJECT_NO_COPY_F, XT_CORE_OBJECT_NO_DESTROY_F);
  if (server->outbox) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
    xt_core_log_trace(server->log, "hnet", "x_case_list_create");
  }

  if (success) {
    if (0 == pthread_mutex_init(&server->outbox_mutex, NULL)) {
      success = xt_core_bool_true;
    } else {
      success = xt_core_bool_false;
      xt_core_log_trace(server->log, "hnet", "pthread_mutex_init");
      xt_case_list_destroy(server->outbox);
    }
  }

  return success;
}

void xt_net_server_system_create_rollback(xt_net_server_system_t *server)
{
  if (server) {
    if (server->name) {
      free(server->name);
    }
    if (server->outbox) {
      xt_case_list_destroy(server->outbox);
    }
    if (server->client_posts) {
      xt_case_set_destroy(server->client_posts);
    }
    if (server->client_exchange) {
      xt_net_exchange_destroy(server->client_exchange);
    }
    if (server->engines) {
      xt_case_list_destroy(server->engines);
    }
    free(server);
  }
}

void xt_net_server_system_destroy(xt_net_server_system_t *server)
{
  assert(server);
  void *message_object;

  join_thread(server, server->server_thread);

  xt_case_list_iterate_start(server->outbox);
  while ((message_object = xt_case_list_iterate_next(server->outbox))) {
    server->messagey->destroy(message_object);
  }
  xt_case_list_destroy(server->outbox);

  xt_case_set_destroy(server->client_posts);
  xt_net_exchange_destroy(server->client_exchange);
  xt_case_list_destroy(server->engines);
  free(server->name);
  free(server);
}

xt_core_bool_t xt_net_server_system_engine_run_thread_quiesce_requested
(xt_net_server_system_t *server, xt_net_engine_id_t engine_id,
    unsigned short thread_index)
{
  return server->engines_array[engine_id]
    ->run_threads_quiesce_requested[thread_index];
}

xt_net_server_system_handle_message_f
xt_net_server_system_get_handler_for_message
(xt_net_server_system_t *server, void *message_object)
{
  assert(server);
  assert(message_object);
  engine_container_t *engine_container;
  xt_net_engine_id_t engine_id;
  unsigned long message_type;
  xt_net_server_system_handle_message_f handler;

  engine_id = server->messagey->get_engine_id(message_object);
  engine_container = server->engines_array[engine_id];
  message_type = server->messagey->get_type(message_object);

  if (message_type < engine_container->message_handlers_size) {
    handler = *(engine_container->message_handlers + message_type);
  } else {
    xt_core_log_enter(server->log, "hnet",
        "server has no handler for %s engine, message %lu",
        xt_net_engine_get_name(engine_container->engine_id,
            server->get_engine_name),
        message_type);
    handler = NULL;
  }

  return handler;
}

xt_config_system_t *xt_net_server_system_get_config_system
(xt_net_server_system_t *server)
{
  return server->config_system;
}

void xt_net_server_system_get_stats(xt_net_server_system_t *server,
    xt_net_server_stats_t *server_stats)
{
  assert(server_stats);

  server_stats->engine_count = engine_count(server);
  server_stats->server_send_message_failure_count
    = server->stats.server_send_message_failure_count;
  server_stats->engine_cant_handle_message_now_count
    = server->stats.engine_cant_handle_message_now_count;
  server_stats->engine_handled_message_count
    = server->stats.engine_handled_message_count;
  server_stats->engine_cant_handle_message_count
    = server->stats.engine_cant_handle_message_count;
  server_stats->discarded_message_for_nonexistent_client_count
    = server->stats.discarded_message_for_nonexistent_client_count;
  server_stats->discarded_message_for_unregistered_engine_count
    = server->stats.discarded_message_for_unregistered_engine_count;
  server_stats->discarded_message_engine_inbox_add_failed_count
    = server->stats.discarded_message_engine_inbox_add_failed_count;
  server_stats->uptime_seconds = time(NULL) - server->create_time;
  server_stats->connected_client_count = xt_case_set_get_size
    (server->client_posts);
  server_stats->max_threads = server->max_threads;
  server_stats->thread_count = server->thread_count;
}

void xt_net_server_system_print_stats(xt_net_server_system_t *server)
{
  xt_net_server_stats_t stats;
  xt_core_time_t *time;
  char *time_string;
  xt_core_dayhour_t dayhour;

  xt_net_server_system_get_stats(server, &stats);

  time = xt_core_time_create(stats.uptime_seconds);
  if (time) {
    xt_core_time_get_dayhour(time, &dayhour);
    time_string = create_string_from_dayhour(&dayhour);
    xt_core_time_destroy(time);
  } else {
    time_string = NULL;
  }

  if (time_string) {
    xt_core_log_enter(server->log, "hnet", "%s server: %lu engines, up %s, "
        "handled %lu messages", server->name, stats.engine_count, time_string,
        stats.engine_handled_message_count);
  }

  if (stats.server_send_message_failure_count > 0) {
    xt_core_log_enter(server->log, "hnet", "%s server_send_message() "
        "failures: %lu", server->name,
        stats.server_send_message_failure_count);
  }

  if (stats.discarded_message_for_unregistered_engine_count > 0) {
    xt_core_log_enter(server->log, "hnet", "%s messages received for "
        "unregistered engines: %lu", server->name,
        stats.discarded_message_for_unregistered_engine_count);
  }

  if (stats.engine_cant_handle_message_count > 0) {
    xt_core_log_enter(server->log, "hnet", "%s instances of engine can't "
        "handle message: %lu", server->name,
        stats.engine_cant_handle_message_count);
  }

  if (stats.engine_cant_handle_message_now_count > 0) {
    xt_core_log_enter(server->log, "hnet", "%s instances of engine can't "
        "handle message now: %lu", server->name,
        stats.engine_cant_handle_message_now_count);
  }

  if (time_string) {
    free(time_string);
  }
}

void xt_net_server_system_process_messages(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id, unsigned short thread_index)
{
  assert(server);
  xt_case_list_t *inbox;
  pthread_mutex_t *inbox_mutex;
  xt_net_message_status_t message_status;
  void *message_object;
  xt_net_server_system_handle_message_f handler;
  unsigned long message_type;
  engine_container_t *engine_container;

  engine_container = server->engines_array[engine_id];
  inbox = engine_container->thread_inboxes[thread_index];
  inbox_mutex = &engine_container->thread_inbox_mutexes[thread_index];

  pthread_mutex_lock(inbox_mutex);
  xt_case_list_iterate_start(inbox);
  while ((message_object = xt_case_list_iterate_next(inbox))) {
    handler = engine_container->iengine->get_handler_for_message
      (engine_container->engine_object, message_object);
    if (handler) {
      message_status
        = (*handler)(engine_container->engine_object, message_object);
      switch (message_status) {
        case XT_NET_MESSAGE_STATUS_HANDLED:
          server->stats.engine_handled_message_count++;
          xt_case_list_iterate_remove(inbox);
          break;
        case XT_NET_MESSAGE_STATUS_CANT_HANDLE:
          server->stats.engine_cant_handle_message_count++;
          xt_case_list_iterate_remove(inbox);
          break;
        case XT_NET_MESSAGE_STATUS_CANT_HANDLE_NOW:
          server->stats.engine_cant_handle_message_now_count++;
          break;
        default:
          break;
      }
    } else {
      message_type = server->messagey->get_type(message_object);
      xt_core_log_enter(server->log, "hnet",
          "server has no handler for %s engine's message %lu",
          xt_net_engine_get_name(engine_container->engine_id,
              server->get_engine_name),
          message_type);
      xt_case_list_iterate_remove(inbox);
    }
  }
  pthread_mutex_unlock(inbox_mutex);
}

xt_core_bool_t xt_net_server_system_register_engine
(xt_net_server_system_t *server, xt_net_engine_id_t engine_id,
    void *custom_server_object, xt_net_engine_iengine_t *iengine,
    unsigned short min_run_threads, unsigned short max_run_threads,
    xt_net_maintain_t maintain_schedule, unsigned long message_type_count)
{
  assert(min_run_threads > 0);
  assert(max_run_threads >= min_run_threads);
  assert(max_run_threads <= MAX_THREADS_PER_ENGINE);
  assert(!server->engines_array[engine_id]);
  engine_container_t *engine_container;
  xt_core_bool_t success;

  if (engine_is_registered(server, engine_id)) {
    success = xt_core_bool_false;
  } else {
    success = xt_core_bool_true;
  }

  if (success) {
    engine_container = create_engine_container(server, engine_id,
        custom_server_object, iengine, min_run_threads, max_run_threads,
        maintain_schedule, message_type_count);
    if (!engine_container) {
      success = xt_core_bool_false;
    }
  }

  if (success) {
    server->engines_array[engine_id] = engine_container;
    if (!xt_case_list_add_last(server->engines, engine_container)) {
      xt_core_log_trace(server->log, "hnet", "x_case_list_add_last");
      success = xt_core_bool_false;
    }
  }

  if (success) {
    success = xt_net_server_system_register_engine_create(engine_container,
        iengine->create, server, custom_server_object);
  }

  return success;
}

xt_core_bool_t xt_net_server_system_register_engine_create
(engine_container_t *engine_container, xt_net_engine_iengine_create_f create,
    xt_net_server_system_t *server, void *custom_server_object)
{
  assert(engine_container);
  xt_core_bool_t success;

  engine_container->engine_object = create(server, custom_server_object);
  if (engine_container->engine_object) {
    success = xt_core_bool_true;
  } else {
    xt_core_log_trace(server->log, "hnet", "create");
    success = xt_core_bool_false;
  }

  return success;
}

void xt_net_server_system_register_message_handler
(xt_net_server_system_t *server, xt_net_engine_id_t engine_id,
    unsigned long message_type,
    xt_net_server_system_handle_message_f message_handler)
{
  assert(server);
  assert(server->engines_array[engine_id]);
  assert(message_type
      < server->engines_array[engine_id]->message_handlers_size);
  engine_container_t *engine_container;

  engine_container = server->engines_array[engine_id];
  *(engine_container->message_handlers + message_type) = message_handler;
}

xt_core_bool_t xt_net_server_system_send_message
(xt_net_server_system_t *server, void *message_object)
{
  xt_core_bool_t success;

  pthread_mutex_lock(&server->outbox_mutex);
  if (xt_case_list_add_last(server->outbox, message_object)) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
    server->stats.server_send_message_failure_count++;
  }
  pthread_mutex_unlock(&server->outbox_mutex);

  return success;
}

void xt_net_server_system_set_unresponsive_client_time_seconds
(xt_net_server_system_t *server,
    unsigned long unresponsive_client_time_seconds)
{
  server->unresponsive_client_time_seconds = unresponsive_client_time_seconds;
}

xt_core_bool_t xt_net_server_system_start(xt_net_server_system_t *server)
{
  xt_core_bool_t success;

  xt_core_stop_requested = xt_core_bool_false;

  if (start(server)) {
    if (create_thread
        (server, &server->server_thread, server_thread, (void *) server)) {
      success = xt_core_bool_true;
    } else {
      success = xt_core_bool_false;
    }
  } else {
    success = xt_core_bool_false;
  }
  if (!success) {
    stop(server);
  }

  return success;
}

xt_case_list_t *xt_net_server_system_take_unprocessed_messages
(xt_net_server_system_t *server)
{
  return NULL;
}

xt_case_list_t *xt_net_server_system_take_unsent_messages
(xt_net_server_system_t *server)
{
  return NULL;
}

xt_core_bool_t join_thread(xt_net_server_system_t *server,
    pthread_t thread)
{
  xt_core_bool_t success;

  if (0 == pthread_join(thread, NULL)) {
    success = xt_core_bool_true;
    server->thread_count--;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

void *maintain_engine(void *maintaining_engine_object)
{
  maintaining_engine_t *maintaining_engine;
  void *engine_object;
  xt_net_engine_id_t engine_id;
  xt_net_server_system_t *server;
  engine_container_t *engine_container;
  xt_sync_period_t *maintenance_period;

  maintaining_engine = maintaining_engine_object;
  engine_object = maintaining_engine->engine_object;
  engine_id = maintaining_engine->engine_id;
  server = maintaining_engine->server;
  engine_container = server->engines_array[engine_id];

  maintenance_period
    = xt_sync_period_create(engine_container->maintain_schedule);
  if (maintenance_period) {
    while (!engine_container->stop_requested) {
      if (xt_sync_period_once(maintenance_period)) {
        maintaining_engine->maintain(engine_object);
      }
      engine_sleep(server, engine_id);
    }
    xt_sync_period_destroy(maintenance_period);
  } else {
    xt_core_log_trace(server->log, "hnet", "x_sync_period_create");
  }

  destroy_maintaining_engine(maintaining_engine);

  return NULL;
}

void manage_threads(xt_net_server_system_t *server)
{
}

void post_messages_to_clients(xt_net_server_system_t *server)
{
  assert(server);
  void *message_object;
  void *client_post_object;
  int client_socket;

  pthread_mutex_lock(&server->outbox_mutex);
  xt_case_list_iterate_start(server->outbox);
  while ((message_object = xt_case_list_iterate_next(server->outbox))) {
    client_socket = server->messagey->get_client_socket(message_object);
    client_post_object = find_client_post(server, client_socket);
    if (client_post_object) {
      if (server->ipost->send_message(client_post_object, message_object)) {
        xt_case_list_iterate_remove(server->outbox);
      } else {
        xt_core_log_trace(server->log, "hnet", "send_message");
      }
    } else {
      xt_case_list_iterate_remove(server->outbox);
      server->stats.discarded_message_for_nonexistent_client_count++;
      server->messagey->destroy(message_object);
      xt_core_log_enter(server->log, "hnet", "server "
          "post_messages_to_clients() discarded "
          "message for nonexistent client %i", client_socket);
    }
  }
  pthread_mutex_unlock(&server->outbox_mutex);
}

void receive_messages_from_client_post(xt_net_server_system_t *server,
    void *post_object)
{
  assert(server);
  assert(post_object);
  void *message_object;
  xt_net_engine_id_t engine_id;
  engine_container_t *engine_container;
  xt_case_list_t *engine_inbox;

  while ((message_object = server->ipost->receive_message(post_object))) {
    engine_id = server->messagey->get_engine_id(message_object);
    if (engine_is_registered(server, engine_id)) {
      engine_container = server->engines_array[engine_id];
      engine_inbox = engine_container->inbox;
      if (!xt_case_list_add_last(engine_inbox, message_object)) {
        xt_core_log_trace(server->log, "hnet", "x_case_list_add_last");
        server->stats.discarded_message_engine_inbox_add_failed_count++;
        server->messagey->destroy(message_object);
      }
    } else {
      xt_core_log_enter(server->log, "hnet",
          "server discarded message type %lu for %s "
          "engine because the engine is not registered",
          server->messagey->get_type(message_object),
          xt_net_engine_get_name(engine_id, server->get_engine_name));
      server->stats.discarded_message_for_unregistered_engine_count++;
      server->messagey->destroy(message_object);
    }
  }
}

void receive_messages_from_clients(xt_net_server_system_t *server)
{
  void *client_post_object;
  xt_case_set_t *client_posts;

  client_posts = server->client_posts;
  xt_case_set_iterate_start(client_posts);
  while ((client_post_object = xt_case_set_iterate_next(client_posts))) {
    receive_messages_from_client_post(server, client_post_object);
  }
}

void *run_engine(void *running_engine_object)
{
  running_engine_t *running_engine;
  void *engine_object;
  xt_net_engine_id_t engine_id;
  xt_net_server_system_t *server;
  engine_container_t *engine_container;
  xt_net_engine_thread_t *engine_thread;

  running_engine = running_engine_object;
  engine_object = running_engine->engine_object;
  engine_id = running_engine->engine_id;
  server = running_engine->server;
  engine_container = server->engines_array[engine_id];
  engine_thread = xt_net_engine_create_thread
    (engine_object, running_engine->thread_index);

  running_engine->start(engine_thread);
  while (!engine_container->stop_requested) {
    running_engine->run(engine_thread);
    engine_sleep(server, engine_id);
  }
  running_engine->stop(engine_thread);

  xt_net_engine_destroy_thread(engine_thread);
  destroy_running_engine(running_engine);

  return NULL;
}

xt_core_bool_t serversocket_bind_listen(xt_net_server_system_t *server)
{
  assert(server);
  xt_core_bool_t success;
  unsigned short port;

  success = xt_core_bool_false;

  for (port = server->min_port;
       (port <= server->max_port) && (!success); port++) {
    server->socket = xt_net_server_socket_create(port);
    if (server->socket >= 0) {
      success = xt_core_bool_true;
      xt_core_log_enter(server->log, "hnet", "listening on port %i", port);
    } else {
      xt_core_log_enter(server->log, "hnet", "x_net_serversocket_create");
    }
  }

  return success;
}

void *server_thread(void *server_object)
{
  xt_net_server_system_t *server;

  server = server_object;

  while (!xt_core_stop_requested) {

    pthread_mutex_lock(&server->client_posts_mutex);
    {
      xt_net_exchange_send_and_receive_messages(server->client_exchange);
      post_messages_to_clients(server);
      receive_messages_from_clients(server);
      close_disconnected_clients(server);
      if (server->unresponsive_client_time_seconds) {
        close_unresponsive_clients(server);
      }
    }
    pthread_mutex_unlock(&server->client_posts_mutex);

    deliver_messages_to_engines(server);

    manage_threads(server);

    usleep(XT_NET_SERVER_SYSTEM_SLEEP_MICROSECONDS);

  }

  stop(server);

  return NULL;
}

xt_core_bool_t start(xt_net_server_system_t *server)
{
  engine_container_t *engine_container;
  xt_net_engine_id_t engine_id;
  xt_core_bool_t success;

  success = xt_core_bool_true;

  signal(SIGINT, handle_signal);
  signal(SIGUSR1, handle_signal);
  signal(SIGUSR2, handle_signal);
  signal(SIGPIPE, SIG_IGN);

  xt_case_list_iterate_start(server->engines);
  while ((engine_container = xt_case_list_iterate_next(server->engines))) {
    engine_id = engine_container->engine_id;
    if (!start_engine(server, engine_id)) {
      success = xt_core_bool_false;
      xt_core_log_trace(server->log, "hnet", "start_engine");
      break;
    }
  }

  if (success) {
    if (!serversocket_bind_listen(server)) {
      success = xt_core_bool_false;
      xt_core_log_trace(server->log, "hnet", "serversocket_bind_listen");
    }
  }

  if (success) {
    if (create_thread(server, &server->accept_thread, accept_thread,
            (void *) server)) {
      server->accept_thread_created = xt_core_bool_true;
    } else {
      success = xt_core_bool_false;
      xt_core_log_trace(server->log, "hnet", "create_thread");
    }
  }

  return success;
}

xt_core_bool_t start_engine(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id)
{
  xt_core_bool_t success;
  unsigned short each_thread;
  engine_container_t *engine_container;

  success = xt_core_bool_true;
  engine_container = server->engines_array[engine_id];

  for (each_thread = 0; each_thread < engine_container->min_run_threads;
       each_thread++) {
    success = start_engine_run_thread(server, engine_id, engine_container,
        each_thread);
    if (success) {
      engine_container->run_thread_count++;
    } else {
      break;
    }
  }
  if (success) {
    success = start_engine_maintain_thread(server, engine_id,
        engine_container);
  }

  return success;
}

xt_core_bool_t start_engine_maintain_thread(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id, engine_container_t *engine_container)
{
  xt_core_bool_t success;
  pthread_t *pthread_address;
  maintaining_engine_t *maintaining_engine;

  pthread_address = &engine_container->maintain_thread;
  maintaining_engine = create_maintaining_engine(server,
      engine_container->engine_object, engine_id,
      engine_container->iengine->maintain);
  if (maintaining_engine) {
    success = create_thread(server, pthread_address, maintain_engine,
        maintaining_engine);
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t start_engine_run_thread(xt_net_server_system_t *server,
    xt_net_engine_id_t engine_id, engine_container_t *engine_container,
    unsigned short thread_index)
{
  xt_core_bool_t success;
  running_engine_t *running_engine;
  pthread_t *pthread_address;

  pthread_address = &engine_container->threads[thread_index];
  running_engine = create_running_engine(server,
      engine_container->engine_object, engine_id,
      engine_container->iengine->start, engine_container->iengine->run,
      engine_container->iengine->stop, thread_index);
  if (running_engine) {
    success
      = create_thread(server, pthread_address, run_engine, running_engine);
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

void stop(xt_net_server_system_t *server)
{
  unsigned short each_thread;
  unsigned short run_thread_count;
  engine_container_t *engine_container;

  /*
    call a function on the engines, telling them to clean up for shutdown?
  */

  if (server->accept_thread_created) {
    join_thread(server, server->accept_thread);
  }

  xt_net_server_socket_destroy(server->socket);

  xt_case_list_iterate_start(server->engines);
  while ((engine_container = xt_case_list_iterate_next(server->engines))) {
    engine_container->stop_requested = xt_core_bool_true;
  }

  xt_case_list_iterate_start(server->engines);
  while ((engine_container = xt_case_list_iterate_next(server->engines))) {
    if (engine_container->iengine->maintain) {
      join_thread(server, engine_container->maintain_thread);
    }
    run_thread_count = engine_container->run_thread_count;
    for (each_thread = 0; each_thread < run_thread_count; each_thread++) {
      join_thread(server, engine_container->threads[each_thread]);
    }
  }
}

xt_core_bool_t threads_maxed_out(xt_net_server_system_t *server)
{
  assert(server);
  xt_core_bool_t maxed_out;

  if (server->thread_count < server->max_threads) {
    maxed_out = xt_core_bool_false;
  } else {
    maxed_out = xt_core_bool_true;
  }

  return maxed_out;
}
