#include "xt/case/array.h"
#include "xt/core/basic/unsigned_long.h"
#include "xt/case/set.h"
#include "xt/core/string.h"
#include "xt/core/tools.h"

struct xt_case_array_t {
  void **array;
  unsigned long array_size;

  unsigned long iterator;
  xt_core_bool_t iterate_remove;
  xt_core_bool_t iterate_first;

  xt_core_object_compare_f compare;
  xt_core_object_copy_f copy;
  xt_core_object_destroy_f destroy;
};

void xt_case_array_add(xt_case_array_t *array, unsigned long index,
    void *object)
{
  assert(array);
  assert(object);
  *(array->array + index) = object;
}

xt_core_bool_t add_to_message(xt_case_array_t *array,
    xt_core_message_t *message,
    xt_core_message_add_to_message_f add_to_message)
{
  assert(array);
  assert(message);
  xt_core_bool_t success;
  long array_size;
  void *object;

  array_size = xt_case_array_get_size(array);
  success = xt_core_message_add_long(message, &array_size);
  xt_case_array_iterate_start(array);
  while ((object = xt_case_array_iterate_next(array))) {
    if (!add_to_message(object, message)) {
      success = xt_core_bool_false;
      break;
    }
  }

  return success;
}

void xt_case_array_clear(xt_case_array_t *array)
{
  assert(array);
  unsigned long index;
  void *object;

  if (array->destroy) {
    for (index = 0; index < array->array_size; index++) {
      object = *(array->array + index);
      if (object) {
        array->destroy(object);
        *(array->array + index) = NULL;
      }
    }
  } else {
    for (index = 0; index < array->array_size; index++) {
      *(array->array + index) = NULL;
    }
  }
}

int xt_case_array_compare(void *array_object_a,
    void *array_object_b)
{
  assert(array_object_a);
  assert(array_object_b);
  xt_case_array_t *array_a;
  xt_case_array_t *array_b;
  int compare_result;
  void *object_a;
  void *object_b;
  unsigned long array_size;
  unsigned long each_element;
  xt_core_object_compare_f compare;

  array_a = array_object_a;
  array_b = array_object_b;

  compare_result = xt_core_basic_unsigned_long_compare(&array_a->array_size,
      &array_b->array_size);
  if (0 == compare_result) {
    array_size = array_a->array_size;
    compare = array_a->compare;
    for (each_element = 0; each_element < array_size; each_element++) {
      object_a = *(array_a->array + each_element);
      object_b = *(array_b->array + each_element);
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

void *xt_case_array_copy(void *array_object)
{
  assert(array_object);
  xt_case_array_t *array;
  xt_case_array_t *array_copy;
  unsigned long index;
  void *object;
  void *object_copy;

  array = array_object;

  array_copy = xt_case_array_create(array->array_size,
      array->compare, array->copy, array->destroy);
  if (array_copy) {
    for (index = 0; index < array->array_size; index++) {
      object = *(array->array + index);
      if (object) {
        object_copy = array->copy(object);
        xt_case_array_add(array_copy, index, object_copy);
      }
    }
  }

  return array_copy;
}

unsigned long xt_case_array_count_non_null_objects
(xt_case_array_t *array)
{
  assert(array);
  unsigned long count;
  unsigned long each_object;

  count = 0;
  for (each_object = 0; each_object < array->array_size; each_object++) {
    if (*(array->array + each_object)) {
      count++;
    }
  }

  return count;
}

xt_case_array_t *xt_case_array_create(unsigned long initial_size,
    xt_core_object_compare_f compare, xt_core_object_copy_f copy, xt_core_object_destroy_f destroy)
{
  assert(compare);
  assert(copy);
  xt_case_array_t *array;
  unsigned long array_memory_size;

  array = malloc(sizeof *array);
  if (array) {

    array->array_size = initial_size;
    array->compare = compare;
    array->copy = copy;
    array->destroy = destroy;

    array_memory_size = initial_size * sizeof(void *);
    array->array = malloc(array_memory_size);
    if (array->array) {
      memset(array->array, '\0', array_memory_size);
    } else {
      xt_core_trace("malloc");
      free(array);
      array = NULL;
    }

  } else {
    xt_core_trace("malloc");
  }

  return array;
}

xt_case_array_t *xt_case_array_create_from_message
(xt_core_object_compare_f compare_object, xt_core_object_copy_f copy,
    xt_core_object_destroy_f destroy, xt_core_message_t *message,
    xt_core_message_create_from_message_f create_from_message)
{
  xt_case_array_t *array;
  long array_size;
  long each_object;
  void *object;

  array_size = xt_core_message_take_long_value(message);
  array = xt_case_array_create(array_size, compare_object, copy,
      destroy);
  if (array) {
    for (each_object = 0; each_object < array_size; each_object++) {
      object = create_from_message(message);
      xt_case_array_add(array, each_object, object);
    }
  } else {
    xt_core_trace("create");
  }

  return array;
}

xt_case_array_t *xt_case_array_create_strings_from_string
(char *string, char *separators)
{
  assert(string);
  assert(separators);
  xt_case_array_t *array;
  char *strtok_context;
  char *token;
  char *token_copy;
  unsigned long index;
  unsigned long field_count;
  char *string_char;
  char *separators_char;

  field_count = 0;
  for (string_char = string; *string_char != '\0'; string_char++) {
    for (separators_char = separators; *separators_char != '\0';
         separators_char++) {
      if (*string_char == *separators_char) {
        field_count++;
        break;
      }
    }
  }
  field_count++;

  array = xt_case_array_create(field_count, xt_core_string_compare,
      xt_core_string_copy, xt_core_string_destroy);
  if (array) {
    index = 0;
    token = strtok_r(string, separators, &strtok_context);
    while (token) {
      token_copy = xt_core_string_copy(token);
      if (token_copy) {
        xt_case_array_add(array, index, token_copy);
        index++;
      } else {
        xt_core_trace("x_core_string_copy");
      }
      token = strtok_r(NULL, separators, &strtok_context);
    }
  } else {
    xt_core_trace("create");
  }

  return array;
}

void xt_case_array_destroy(void *array_object)
{
  assert(array_object);
  xt_case_array_t *array;

  array = array_object;

  xt_case_array_clear(array);
  free(array->array);
  free(array);
}

void xt_case_array_expunge_set(xt_case_array_t *array,
    xt_case_set_t *expunge_these)
{
  assert(array);
  assert(expunge_these);
  void *object;
  unsigned long each_object;

  for (each_object = 0; each_object < array->array_size; each_object++) {
    object = *(array->array + each_object);
    if (xt_case_set_find(expunge_these, object)) {
      if (array->destroy) {
        array->destroy(object);
      }
      *(array->array + each_object) = NULL;
    }
  }
}

void xt_case_array_expunge_set_inverse(xt_case_array_t *array,
    xt_case_set_t *expunge_all_but_these)
{
  assert(array);
  assert(expunge_all_but_these);
  void *object;
  unsigned long each_object;

  for (each_object = 0; each_object < array->array_size; each_object++) {
    object = *(array->array + each_object);
    if (!xt_case_set_find(expunge_all_but_these, object)) {
      if (array->destroy) {
        array->destroy(object);
      }
      *(array->array + each_object) = NULL;
    }
  }
}

void *xt_case_array_find(xt_case_array_t *array,
    unsigned long index)
{
  return *(array->array + index);
}

void *xt_case_array_find_last(xt_case_array_t *array)
{
  assert(array);
  void *object;

  if (array->array_size > 0) {
    object = *(array->array + (array->array_size - 1));
  } else {
    object = NULL;
  }

  return object;
}

void *xt_case_array_find_first(xt_case_array_t *array)
{
  assert(array);
  void *object;

  if (array->array_size > 0) {
    object = *(array->array);
  } else {
    object = NULL;
  }

  return object;
}

void *xt_case_array_find_random(xt_case_array_t *array)
{
  assert(array);
  unsigned long random_index;
  void *random_object;

  random_index = random() % array->array_size;
  random_object = *(array->array + random_index);

  return random_object;
}

void **xt_case_array_get_array(xt_case_array_t *array)
{
  return array->array;
}

char *xt_case_array_get_as_delimited_string(xt_case_array_t *array,
    xt_core_object_get_as_string_f get_as_string, char *delimiter)
{
  assert(array);
  assert(get_as_string);
  assert(delimiter);
  char *string;
  void *object;
  char *object_string;
  unsigned long object_index;
  unsigned long last_object_index;
  xt_core_bool_t success;

  string = NULL;
  last_object_index = array->array_size - 1;
  success = xt_core_bool_true;

  object_index = 0;
  xt_case_array_iterate_start(array);
  while ((object = xt_case_array_iterate_next(array))) {
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

xt_core_object_compare_f xt_case_array_get_compare
(xt_case_array_t *array)
{
  return array->compare;
}

xt_core_object_copy_f xt_case_array_get_copy
(xt_case_array_t *array)
{
  return array->copy;
}

xt_core_object_destroy_f xt_case_array_get_destroy
(xt_case_array_t *array)
{
  return array->destroy;
}

unsigned long xt_case_array_get_size(xt_case_array_t *array)
{
  return array->array_size;
}

void *xt_case_array_iterate_next(xt_case_array_t *array)
{
  assert(array);
  void *next_object;

  if (array->iterator < array->array_size) {
    if (array->iterate_first) {
      next_object = *(array->array + array->iterator);
      array->iterate_first = xt_core_bool_false;
    } else {
      if (array->iterate_remove) {
        xt_case_array_remove(array, array->iterator);
        array->iterator++;
        array->iterate_remove = xt_core_bool_false;
      } else {
        array->iterator++;
      }
      if (array->iterator < array->array_size) {
        next_object = *(array->array + array->iterator);
      } else {
        next_object = NULL;
      }
    }
  } else {
    next_object = NULL;
  }

  return next_object;
}

void xt_case_array_iterate_remove(xt_case_array_t *array)
{
  array->iterate_remove = xt_core_bool_true;
}

void xt_case_array_iterate_start(xt_case_array_t *array)
{
  assert(array);

  array->iterator = 0;
  array->iterate_remove = xt_core_bool_false;
  array->iterate_first = xt_core_bool_true;
}

void xt_case_array_print(xt_case_array_t *array)
{
  assert(array);
  unsigned long each_element;

  for (each_element = 0; each_element < array->array_size; each_element++) {
    printf("[%p]", *(array->array + each_element));
  }
  printf("\n");
}

void xt_case_array_remove(xt_case_array_t *array,
    unsigned long index)
{
  assert(array);
  void *existing_object;

  if (array->destroy) {
    existing_object = *(array->array + index);
    if (existing_object) {
      array->destroy(existing_object);
    }
  }
  *(array->array + index) = NULL;
}

void xt_case_array_replace(xt_case_array_t *array,
    unsigned long index, void *object)
{
  assert(array);
  assert(object);

  xt_case_array_remove(array, index);
  xt_case_array_add(array, index, object);
}

void xt_case_array_replace_random(xt_case_array_t *array,
    void *object)
{
  assert(array);
  assert(object);
  unsigned long random_index;

  random_index = random() % array->array_size;
  xt_case_array_replace(array, random_index, object);
}

xt_core_bool_t xt_case_array_resize(xt_case_array_t *array,
    unsigned long new_size)
{
  assert(array);
  xt_core_bool_t success;
  unsigned long index;
  void **new_array;

  success = xt_core_bool_true;

  if (new_size < array->array_size) {
    success = xt_core_bool_true;
    for (index = new_size; index < array->array_size; index++) {
      xt_case_array_remove(array, index);
    }

  } else if (new_size > array->array_size) {
    new_array = realloc(array->array, new_size * sizeof(void *));
    if (new_array) {
      success = xt_core_bool_true;
      array->array = new_array;
      memset(array->array + array->array_size, '\0',
          new_size - array->array_size);
    } else {
      success = xt_core_bool_false;
    }

  }

  array->array_size = new_size;

  return success;
}

void xt_case_array_sort(xt_case_array_t *array)
{
  qsort(array->array, array->array_size, sizeof(void *),
      (xt_core_object_compare_const_f) array->compare);
}
