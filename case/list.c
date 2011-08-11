#include "xt/case/array.h"
#include "xt/case/list.h"
#include "xt/core/basic/unsigned_long.h"
#include "xt/core/string.h"
#include "xt/core/tools.h"

struct list_object_t;
typedef struct list_object_t list_object_t;

struct list_object_t {
  void *object;
  list_object_t *before;
  list_object_t *after;
};

struct xt_case_list_t {
  list_object_t *first;
  list_object_t *last;
  unsigned long size;
  unsigned long size_limit;

  xt_core_object_compare_f compare;
  xt_core_object_copy_f copy;
  xt_core_object_destroy_f destroy;

  list_object_t *iterator;
  xt_core_bool_t iterate_remove;
  xt_core_bool_t iterate_first;

  pthread_mutex_t mutex;
};

static void connect_list_objects(list_object_t *object_a,
    list_object_t *object_b);

static list_object_t *list_object_create(void *object,
    list_object_t *before,
    list_object_t *after);

static void list_object_destroy(xt_case_list_t *list,
    list_object_t *list_object);

static void remove_list_object(xt_case_list_t *list,
    list_object_t *list_object);

static void update_first_and_last_on_remove(xt_case_list_t *list,
    list_object_t *list_object);

void connect_list_objects(list_object_t *object_a, list_object_t *object_b)
{

  if (object_a) {
    object_a->after = object_b;
  }
  if (object_b) {
    object_b->before = object_a;
  }
}

xt_core_bool_t xt_case_list_absorb(xt_case_list_t *list,
    xt_case_list_t *absorb_these)
{
  assert(list);
  assert(absorb_these);
  xt_core_bool_t success;
  void *object;

  success = xt_core_bool_true;

  xt_case_list_iterate_start(absorb_these);
  while ((object = xt_case_list_iterate_next(absorb_these))) {
    if (!xt_case_list_add_last(list, object)) {
      success = xt_core_bool_false;
      xt_core_trace("x_case_list_add_last");
    }
  }

  return success;
}

xt_core_bool_t xt_case_list_add_to_message(xt_case_list_t *list,
    xt_core_message_t *message,
    xt_core_message_add_to_message_f add_to_message)
{
  xt_core_bool_t success;
  long list_size;
  void *object;

  list_size = xt_case_list_get_size(list);
  success = xt_core_message_add_long(message, &list_size);
  xt_case_list_iterate_start(list);
  while ((object = xt_case_list_iterate_next(list))) {
    if (!add_to_message(object, message)) {
      success = xt_core_bool_false;
      break;
    }
  }

  return success;
}

xt_core_bool_t xt_case_list_add_first(xt_case_list_t *list, void *object)
{
  assert(list);
  assert(object);
  xt_core_bool_t success;
  list_object_t *old_first;
  list_object_t *new_list_object;

  if (list->size_limit) {
    if (list->size == list->size_limit) {
      xt_case_list_remove_last(list);
    }
  }

  if (0 != list->size) {
    old_first = list->first;
    new_list_object = list_object_create(object, NULL, old_first);
    if (new_list_object) {
      old_first->before = new_list_object;
      list->first = new_list_object;
      list->size++;
      success = xt_core_bool_true;
    } else {
      success = xt_core_bool_false;
    }
  } else {
    list->first = list_object_create(object, NULL, NULL);
    if (list->first) {
      list->last = list->first;
      list->size = 1;
      success = xt_core_bool_true;
    } else {
      success = xt_core_bool_false;
    }
  }

  return success;
}

xt_core_bool_t xt_case_list_add_last(xt_case_list_t *list, void *object)
{
  assert(list);
  assert(object);
  xt_core_bool_t success;
  list_object_t *old_last;
  list_object_t *new_list_object;

  if (list->size_limit) {
    if (list->size == list->size_limit) {
      xt_case_list_remove_first(list);
    }
  }

  if (0 != list->size) {
    old_last = list->last;
    new_list_object = list_object_create(object, old_last, NULL);
    if (new_list_object) {
      old_last->after = new_list_object;
      list->last = new_list_object;
      list->size++;
      success = xt_core_bool_true;
    } else {
      success = xt_core_bool_false;
    }
  } else {
    list->first = list_object_create(object, NULL, NULL);
    if (list->first) {
      list->last = list->first;
      list->size = 1;
      success = xt_core_bool_true;
    } else {
      success = xt_core_bool_false;
    }
  }

  return success;
}

void xt_case_list_clear(xt_case_list_t *list)
{
  assert(list);

  xt_case_list_iterate_start(list);
  while (xt_case_list_iterate_next(list)) {
    xt_case_list_iterate_remove(list);
  }

  assert(!list->first);
  assert(!list->last);
  assert(0 == list->size);
}

int xt_case_list_compare(void *list_object_a, void *list_object_b)
{
  assert(list_object_a);
  assert(list_object_b);
  xt_case_list_t *list_a;
  xt_case_list_t *list_b;
  int compare_result;
  void *object_a;
  void *object_b;
  xt_core_object_compare_f compare;

  list_a = list_object_a;
  list_b = list_object_b;

  compare_result
    = xt_core_basic_unsigned_long_compare(&list_a->size, &list_b->size);
  if (0 == compare_result) {
    compare = list_a->compare;

    xt_case_list_iterate_start(list_a);
    xt_case_list_iterate_start(list_b);
    while ((object_a = xt_case_list_iterate_next(list_a))) {
      object_b = xt_case_list_iterate_next(list_b);
      if (object_a && object_b) {
        compare_result = compare(object_a, object_b);
      } else if (!object_a && object_b) {
        compare_result = -1;
      } else if (object_a && !object_b) {
        compare_result = 1;
      } else {
        compare_result = 0;
      }
      if (0 != compare_result) {
        break;
      }
    }
  }

  return compare_result;
}

void *xt_case_list_copy(void *list_object)
{
  assert(list_object);
  xt_case_list_t *list;
  xt_case_list_t *copy;
  void *original_object;
  void *copied_object;

  list = list_object;

  copy = xt_case_list_create(list->compare, list->copy,
      list->destroy);
  if (copy) {
    xt_case_list_iterate_start(list);
    while ((original_object = xt_case_list_iterate_next(list))) {
      copied_object = list->copy(original_object);
      if (copied_object) {
        if (!xt_case_list_add_last(copy, copied_object)) {
          xt_core_trace("x_case_list_add_last");
          list->destroy(copied_object);
          xt_case_list_destroy(copy);
          copy = NULL;
          break;
        }
      } else {
        xt_core_trace("list->copy");
        xt_case_list_destroy(copy);
        copy = NULL;
        break;
      }
    }
  } else {
    xt_core_trace("x_case_list_create");
  }

  return copy;
}

xt_case_list_t *xt_case_list_create(xt_core_object_compare_f compare,
    xt_core_object_copy_f copy, xt_core_object_destroy_f destroy)
{
  xt_case_list_t *list;

  list = malloc(sizeof *list);
  if (list) {
    list->compare = compare;
    list->copy = copy;
    list->destroy = destroy;
    list->first = NULL;
    list->last = NULL;
    list->size = 0;
    list->size_limit = 0;
    list->iterate_remove = xt_core_bool_false;
    if (0 != pthread_mutex_init(&list->mutex, NULL)) {
      xt_core_trace("pthread_mutex_init");
    }
  }

  return list;
}

xt_case_list_t *xt_case_list_create_from_array_n(xt_case_array_t *array,
    unsigned long max_size)
{
  assert(array);
  xt_case_list_t *list;
  void *object;
  void *object_copy;
  unsigned long object_index;
  xt_core_object_compare_f compare;
  xt_core_object_copy_f copy;
  xt_core_object_destroy_f destroy;
  unsigned long array_size;

  compare = xt_case_array_get_compare(array);
  copy = xt_case_array_get_copy(array);
  destroy = xt_case_array_get_destroy(array);
  array_size = xt_case_array_get_size(array);

  list = xt_case_list_create(compare, copy, destroy);
  if (list) {
    object_index = 0;
    while ((list->size < max_size) && (object_index < array_size)) {
      object = xt_case_array_find(array, object_index);
      if (object) {
        object_copy = copy(object);
        if (object_copy) {
          if (!xt_case_list_add_last(list, object_copy)) {
            xt_core_trace("x_case_list_add_last");
          }
        } else {
          xt_core_trace("copy");
        }
      }
      object_index++;
    }
  } else {
    xt_core_trace("x_case_list_create");
  }

  return list;
}

xt_case_list_t *xt_case_list_create_from_message
(xt_core_object_compare_f compare, xt_core_object_copy_f copy,
    xt_core_object_destroy_f destroy, xt_core_message_t *message,
    xt_core_message_create_from_message_f create_from_message)
{
  xt_case_list_t *list;
  long list_size;
  long each_object;
  void *object;

  list = xt_case_list_create(compare, copy, destroy);
  if (list) {
    list_size = xt_core_message_take_long_value(message);
    for (each_object = 0; each_object < list_size; each_object++) {
      object = create_from_message(message);
      if (!xt_case_list_add_last(list, object)) {
        xt_core_trace("x_case_list_add_last");
      }
    }
  } else {
    xt_core_trace("x_case_list_create() failed");
  }

  return list;
}

xt_case_list_t *xt_case_list_create_strings_from_string(char *string,
    const char *separators)
{
  assert(string);
  assert(separators);
  xt_case_list_t *list;
  char *strtok_context;
  char *token;
  char *token_copy;

  list = xt_case_list_create(xt_core_string_compare, xt_core_string_copy,
      xt_core_string_destroy);
  if (list) {
    token = strtok_r(string, separators, &strtok_context);
    while (token) {
      token_copy = xt_core_string_copy(token);
      if (token_copy) {
        if (!xt_case_list_add_last(list, token_copy)) {
          xt_core_trace("x_case_list_add_last");
          xt_core_string_destroy(token_copy);
          break;
        }
      } else {
        xt_core_trace("x_core_string_copy");
      }
      token = strtok_r(NULL, separators, &strtok_context);
    }
  } else {
    xt_core_trace("x_case_list_create");
  }

  return list;
}

void xt_case_list_destroy(void *list_object)
{
  assert(list_object);
  xt_case_list_t *list;

  list = list_object;

  xt_case_list_clear(list);
  if (0 != pthread_mutex_destroy(&list->mutex)) {
    xt_core_trace("pthread_mutex_destroy");
  }
  free(list);

}

void xt_case_list_dont_destroy_objects(xt_case_list_t *list)
{
  list->destroy = NULL;
}

void *xt_case_list_find_at(xt_case_list_t *list,
    unsigned long index)
{
  /*
    TODO: re-implement efficiently: once they call this function once on a
    xt_core_list, start maintaining an array or other structure that makes
    calls to xt_case_list_find_at() efficient
  */

  assert(list);
  void *object;
  list_object_t *list_object;
  unsigned int list_object_index;

  if (index < list->size) {
    list_object = list->first;
    for (list_object_index = 0; list_object_index < index;
         list_object_index++) {
      list_object = list_object->after;
    }
    object = list_object->object;
  } else {
    object = NULL;
  }

  return object;
}

void *xt_case_list_find_first(xt_case_list_t *list)
{
  assert(list);
  void *object;

  if (list->first) {
    object = list->first->object;
  } else {
    object = NULL;
  }

  return object;
}

void *xt_case_list_find_last(xt_case_list_t *list)
{
  assert(list);
  void *object;

  if (list->last) {
    object = list->last->object;
  } else {
    object = NULL;
  }

  return object;
}

void *xt_case_list_find_random(xt_case_list_t *list)
{
  assert(list);
  void *object;
  unsigned int random_index;

  if (list->size > 0) {
    random_index = random() % list->size;
    object = xt_case_list_find_at(list, random_index);
  } else {
    object = NULL;
  }

  assert((0 == list->size) || object);
  return object;
}

char *xt_case_list_get_as_delimited_string(xt_case_list_t *list,
    xt_core_object_get_as_string_f get_as_string, const char *delimiter)
{
  assert(list);
  assert(get_as_string);
  assert(delimiter);
  char *string;
  void *object;
  char *object_string;
  unsigned long object_index;
  unsigned long last_object_index;
  xt_core_bool_t success;

  string = NULL;
  last_object_index = list->size - 1;
  success = xt_core_bool_true;

  object_index = 0;
  xt_case_list_iterate_start(list);
  while ((object = xt_case_list_iterate_next(list))) {
    object_string = get_as_string(object);
    if (object_string) {
      string = xt_core_string_append(string, object_string);
      free(object_string);
      if (string) {
        if (object_index != last_object_index) {
          string = xt_core_string_append(string, delimiter);
          if (!string) {
            success = xt_core_bool_false;
            xt_core_trace("string_append");
            break;
          }
        }
      } else {
        success = xt_core_bool_false;
        xt_core_trace("string_append");
        break;
      }
    } else {
      success = xt_core_bool_false;
      xt_core_trace("get_as_string");
      break;
    }
    object_index++;
  }

  if (!success && string) {
    free(string);
    string = NULL;
  }

  return string;
}

char *xt_case_list_get_as_string(void *list_object)
{
  return NULL;
}

unsigned long xt_case_list_get_size(xt_case_list_t *list)
{
  return list->size;
}

void xt_case_list_init_iobject(xt_core_iobject_t *iobject)
{
  assert(iobject);

  iobject->get_as_string = xt_case_list_get_as_string;
  iobject->compare = xt_case_list_compare;
  iobject->copy = xt_case_list_copy;
  iobject->destroy = xt_case_list_destroy;
}

void *xt_case_list_iterate_next(xt_case_list_t *list)
{
  assert(list);
  void *next_object;
  list_object_t *successor;

  if (list->iterator) {
    if (list->iterate_first) {
      next_object = list->iterator->object;
      list->iterate_first = xt_core_bool_false;
    } else {
      if (list->iterate_remove) {
        successor = list->iterator->after;
        remove_list_object(list, list->iterator);
        list->iterator = successor;
        list->iterate_remove = xt_core_bool_false;
      } else {
        list->iterator = list->iterator->after;
      }
      if (list->iterator) {
        next_object = list->iterator->object;
      } else {
        next_object = NULL;
      }
    }
  } else {
    next_object = NULL;
  }

  return next_object;
}

void xt_case_list_iterate_remove(xt_case_list_t *list)
{
  list->iterate_remove = xt_core_bool_true;
}

void xt_case_list_iterate_start(xt_case_list_t *list)
{
  assert(list);

  list->iterator = list->first;
  list->iterate_remove = xt_core_bool_false;
  list->iterate_first = xt_core_bool_true;

  assert(list->iterator == list->first);
}

void xt_case_list_lock(xt_case_list_t *list)
{
  pthread_mutex_lock(&list->mutex);
}

void xt_case_list_print(void *list_object)
{
  xt_core_trace_exit("TODO: implement");
}

xt_core_bool_t xt_case_list_remove_first(xt_case_list_t *list)
{
  assert(list);
  xt_core_bool_t success;
  list_object_t *first;

  first = list->first;

  if (first) {
    update_first_and_last_on_remove(list, first);
    list_object_destroy(list, first);
    list->size--;
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_case_list_remove_last(xt_case_list_t *list)
{
  assert(list);
  xt_core_bool_t success;
  list_object_t *last;

  last = list->last;

  if (last) {
    update_first_and_last_on_remove(list, last);
    list_object_destroy(list, last);
    list->size--;
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

void xt_case_list_set_size_limit(xt_case_list_t *list,
    unsigned long size_limit)
{
  list->size_limit = size_limit;
}

void xt_case_list_unlock(xt_case_list_t *list)
{
  pthread_mutex_unlock(&list->mutex);
}

list_object_t *list_object_create(void *object, list_object_t *before,
    list_object_t *after)
{
  assert(object);
  list_object_t *list_object;

  list_object = malloc(sizeof *list_object);
  if (list_object) {
    list_object->object = object;
    list_object->before = before;
    list_object->after = after;
  }

  return list_object;
}

void list_object_destroy(xt_case_list_t *list, list_object_t *list_object)
{
  assert(list);
  assert(list_object);

  if (list->destroy) {
    list->destroy(list_object->object);
  }
  free(list_object);

}

void remove_list_object(xt_case_list_t *list, list_object_t *list_object)
{
  connect_list_objects(list_object->before, list_object->after);
  update_first_and_last_on_remove(list, list_object);
  list_object_destroy(list, list_object);
  list->size--;
}

void update_first_and_last_on_remove(xt_case_list_t *list,
    list_object_t *list_object)
{
  assert(list_object);

  if (list_object == list->first) {
    list->first = list_object->after;
    if (list_object->after) {
      list_object->after->before = NULL;
    }
  }
  if (list_object == list->last) {
    list->last = list_object->before;
    if (list_object->before) {
      list_object->before->after = NULL;
    }
  }
}
