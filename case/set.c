#include "xt/case/array.h"
#include "xt/core/basic/long.h"
#include "xt/case/set.h"

#if defined XT_CASE_SET_IMPL_BINARY_TREE
#include "xt/case/set.impl.binary_tree.c"
#elif defined XT_CASE_SET_IMPL_HASH_TABLE
#include "xt/case/set.imple.hash_table.c"
#elif defined XT_CASE_SET_IMPL_HASHA
#include "xt/case/set.impl.hasha.c"
#elif defined XT_CASE_SET_IMPL_IMPL_MBIN
#include "xt/case/set.impl.mbin.c"
#elif defined XT_CASE_SET_SKIP_IMPL_LIST
#include "xt/case/set.impl.skip_list.c"
#else
#include "xt/case/set.impl.binary_tree.c"
#endif

xt_core_bool_t xt_case_set_absorb(xt_case_set_t *set,
    xt_case_set_t *absorb_these)
{
  assert(set);
  assert(absorb_these);
  void *object;
  xt_core_bool_t success;

  success = xt_core_bool_true;

  xt_case_set_iterate_start(absorb_these);
  while ((object = xt_case_set_iterate_next(absorb_these))) {
    if (!xt_case_set_add(set, object)) {
      success = xt_core_bool_false;
      xt_core_trace("x_case_set_add");
    }
  }

  return success;
}

xt_core_bool_t xt_case_set_absorb_array(xt_case_set_t *set,
    xt_case_array_t *absorb_these)
{
  assert(set);
  assert(absorb_these);
  void *object;
  xt_core_bool_t success;

  success = xt_core_bool_true;

  xt_case_array_iterate_start(absorb_these);
  while ((object = xt_case_array_iterate_next(absorb_these))) {
    if (!xt_case_set_add(set, object)) {
      success = xt_core_bool_false;
      xt_core_trace("x_case_set_add");
    }
  }

  return success;
}

xt_core_bool_t xt_case_set_absorb_copy(xt_case_set_t *set,
    xt_case_set_t *absorb_these)
{
  assert(set);
  assert(absorb_these);
  assert(xt_case_set_get_iobject(set)->destroy);
  void *object;
  void *object_copy;
  xt_core_bool_t success;
  xt_core_iobject_t *iobject = xt_case_set_get_iobject(set);

  success = xt_core_bool_true;

  xt_case_set_iterate_start(absorb_these);
  while ((object = xt_case_set_iterate_next(absorb_these))) {
    object_copy = iobject->copy(object);
    if (object_copy) {
      if (!xt_case_set_add(set, object_copy)) {
        iobject->destroy(object_copy);
      }
    } else {
      success = xt_core_bool_false;
      xt_core_trace("copy");
    }
  }

  return success;
}

xt_core_bool_t xt_case_set_absorb_list_copy(xt_case_set_t *set,
    xt_case_list_t *absorb_these)
{
  assert(set);
  assert(absorb_these);
  assert(xt_case_set_get_iobject(set)->destroy);
  void *object;
  void *object_copy;
  xt_core_bool_t success;
  xt_core_iobject_t *iobject = xt_case_set_get_iobject(set);

  success = xt_core_bool_true;

  xt_case_list_iterate_start(absorb_these);
  while ((object = xt_case_list_iterate_next(absorb_these))) {
    object_copy = iobject->copy(object);
    if (object_copy) {
      if (!xt_case_set_add(set, object_copy)) {
        iobject->destroy(object_copy);
      }
    } else {
      success = xt_core_bool_false;
      xt_core_trace("copy");
    }
  }

  return success;
}

xt_core_bool_t xt_case_set_add_to_message(xt_case_set_t *set,
    xt_core_message_t *message,
    xt_core_message_add_to_message_f add_to_message)
{
  xt_core_bool_t success;
  long set_size;
  void *object;

  set_size = xt_case_set_get_size(set);
  success = xt_core_message_add_long(message, &set_size);
  xt_case_set_iterate_start(set);
  while ((object = xt_case_set_iterate_next(set))) {
    if (!add_to_message(object, message)) {
      success = xt_core_bool_false;
      break;
    }
  }

  return success;
}

void xt_case_set_clear(xt_case_set_t *set)
{
  assert(set);

  xt_case_set_iterate_start(set);
  while (xt_case_set_iterate_next(set)) {
    xt_case_set_iterate_remove(set);
  }
}

int xt_case_set_compare(void *set_object_a,
    void *set_object_b)
{
  assert(set_object_a);
  assert(set_object_b);
  xt_case_set_t *set_a;
  xt_case_set_t *set_b;
  int compare_result;
  void *object_a;
  void *object_b;
  unsigned long size_a;
  unsigned long size_b;
  xt_core_object_compare_f compare;

  set_a = set_object_a;
  set_b = set_object_b;

  size_a = xt_case_set_get_size(set_a);
  size_b = xt_case_set_get_size(set_b);

  compare_result = xt_core_basic_long_compare(&size_a, &size_b);
  if (0 == compare_result) {
    compare = xt_case_set_get_iobject(set_a)->compare;
    xt_case_set_iterate_start(set_a);
    xt_case_set_iterate_start(set_b);
    while ((object_a = xt_case_set_iterate_next(set_a))) {
      object_b = xt_case_set_iterate_next(set_b);
      compare_result = compare(object_a, object_b);
      if (0 != compare_result) {
        break;
      }
    }
  }

  return compare_result;
}

xt_core_bool_t xt_case_set_contains(xt_case_set_t *superset,
    xt_case_set_t *subset)
{
  assert(superset);
  assert(subset);
  xt_core_bool_t contains_it;
  void *object;

  contains_it = xt_core_bool_true;

  xt_case_set_iterate_start(subset);
  while ((object = xt_case_set_iterate_next(subset))) {
    if (!xt_case_set_find(superset, object)) {
      contains_it = xt_core_bool_false;
      break;
    }
  }

  return contains_it;
}

void *xt_case_set_copy(void *set_object)
{
  assert(set_object);
  xt_case_set_t *set = set_object;
  xt_case_set_t *copy;
  void *original_object;
  void *copied_object;
  xt_core_iobject_t *iobject = xt_case_set_get_iobject(set);

  copy = xt_case_set_create(iobject);
  if (copy) {
    xt_case_set_iterate_start(set);
    while ((original_object = xt_case_set_iterate_next(set))) {
      copied_object = iobject->copy(original_object);
      if (copied_object) {
        if (!xt_case_set_add(copy, copied_object)) {
          xt_core_trace("x_case_set_add");
          iobject->destroy(copied_object);
          xt_case_set_destroy(copy);
          copy = NULL;
          break;
        }
      } else {
        xt_core_trace("copy");
        xt_case_set_destroy(copy);
        copy = NULL;
        break;
      }
    }
  } else {
    xt_core_trace("x_case_set_create");
  }

  return copy;
}

xt_case_set_t *xt_case_set_create_from_difference(xt_case_set_t *set_a,
    xt_case_set_t *set_b)
{
  assert(set_a);
  assert(set_b);
  xt_case_set_t *difference;

  difference = xt_case_set_copy(set_a);
  if (difference) {
    if (!xt_case_set_expunge(difference, set_b)) {
      xt_core_trace("x_case_set_expunge");
      xt_case_set_destroy(difference);
      difference = NULL;
    }
  } else {
    xt_core_trace("x_case_set_copy");
  }

  return difference;
}

xt_case_set_t *xt_case_set_create_from_message
(xt_core_iobject_t *iobject, xt_core_message_t *message,
    xt_core_message_create_from_message_f create_from_message)
{
  assert(iobject);
  assert(message);
  assert(create_from_message);
  xt_case_set_t *set;
  long set_size;
  long each_object;
  void *object;

  set = xt_case_set_create(iobject);
  if (set) {
    set_size = xt_core_message_take_long_value(message);
    for (each_object = 0; each_object < set_size; each_object++) {
      object = create_from_message(message);
      if (!xt_case_set_add(set, object)) {
        xt_core_trace("x_case_set_add");
      }
    }
  } else {
    xt_core_trace("x_case_set_create");
  }

  return set;
}

xt_case_set_t *xt_case_set_create_from_union(xt_case_set_t *set_a,
    xt_case_set_t *set_b)
{
  assert(set_a);
  assert(set_b);
  xt_case_set_t *union_set;
  void *object;

  union_set = xt_case_set_copy(set_a);
  if (union_set) {
    xt_case_set_iterate_start(set_b);
    while ((object = xt_case_set_iterate_next(set_b))) {
      xt_case_set_add(union_set, object);
    }
  } else {
    xt_core_trace("x_case_set_copy");
  }

  return union_set;
}

void xt_case_set_dont_destroy_objects(xt_case_set_t *set)
{
  xt_case_set_get_iobject(set)->destroy = NULL;
}

xt_core_bool_t xt_case_set_expunge(xt_case_set_t *set,
    xt_case_set_t *expunge_these)
{
  assert(set);
  assert(expunge_these);
  xt_core_bool_t success;
  void *object;

  xt_case_set_iterate_start(expunge_these);
  while ((object = xt_case_set_iterate_next(expunge_these))) {
    xt_case_set_remove(set, object);
  }

  /*
    TODO: return codes in xt_core_set interface don't allow us to determine
    success or failure here
  */
  success = xt_core_bool_true;

  return success;
}

void *xt_case_set_find_copy(xt_case_set_t *set, void *decoy_object)
{
  void *found_object;
  void *found_object_copy;

  found_object = xt_case_set_find(set, decoy_object);
  if (found_object) {
    found_object_copy = xt_case_set_get_iobject(set)->copy(found_object);
    if (!found_object_copy) {
      xt_core_trace("copy");
    }
  } else {
    found_object_copy = NULL;
  }

  return found_object_copy;
}

void *xt_case_set_find_take(xt_case_set_t *set,
    void *decoy_object)
{
  void *object;

  object = xt_case_set_find(set, decoy_object);
  if (object) {
    if (!xt_case_set_remove(set, decoy_object)) {
      xt_core_trace("x_case_set_remove");
    }
  }

  return object;
}

xt_case_array_t *xt_case_set_get_as_array(xt_case_set_t *set)
{
  assert(set);
  xt_case_array_t *array;
  void *object;
  unsigned long object_count;
  unsigned long object_index;
  xt_core_iobject_t *iobject = xt_case_set_get_iobject(set);

  object_count = xt_case_set_get_size(set);

  array = xt_case_array_create(object_count, iobject->compare,
      iobject->copy, XT_CORE_OBJECT_NO_DESTROY_F);
  if (array) {
    object_index = 0;
    xt_case_set_iterate_start(set);
    while ((object = xt_case_set_iterate_next(set))) {
      xt_case_array_add(array, object_index, object);
      object_index++;
    }
  } else {
    xt_core_trace("x_case_array_create");
  }

  return array;
}

xt_case_list_t *xt_case_set_get_as_list(xt_case_set_t *set)
{
  assert(set);
  xt_case_list_t *list;
  void *object;
  xt_core_iobject_t *iobject = xt_case_set_get_iobject(set);

  list = xt_case_list_create(iobject->compare, iobject->copy,
      XT_CORE_OBJECT_NO_DESTROY_F);
  if (list) {
    xt_case_set_iterate_start(set);
    while ((object = xt_case_set_iterate_next(set))) {
      xt_case_list_add_last(list, object);
    }
  } else {
    xt_core_trace("x_case_list_create");
  }

  return list;
}

char *xt_case_set_get_as_delimited_string(xt_case_set_t *set,
    xt_core_object_get_as_string_f get_as_string, const char *delimiter)
{
  assert(set);
  assert(get_as_string);
  assert(delimiter);
  char *string;
  void *object;
  char *object_string;
  unsigned long object_index;
  unsigned long last_object_index;
  xt_core_bool_t success;

  string = NULL;
  last_object_index = xt_case_set_get_size(set) - 1;
  success = xt_core_bool_true;

  object_index = 0;
  xt_case_set_iterate_start(set);
  while ((object = xt_case_set_iterate_next(set))) {
    object_string = get_as_string(object);
    if (object_string) {
      string = xt_core_string_append(string, object_string);
      free(object_string);
      if (string) {
        if (object_index != last_object_index) {
          string = xt_core_string_append(string, delimiter);
          if (!string) {
            success = xt_core_bool_false;
            xt_core_trace("x_core_string_append");
            break;
          }
        }
      } else {
        success = xt_core_bool_false;
        xt_core_trace("x_core_string_append");
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

xt_core_bool_t xt_case_set_overlaps(xt_case_set_t *set_a,
    xt_case_set_t *set_b)
{
  assert(set_a);
  assert(set_b);
  xt_core_bool_t overlaps;
  void *object;
  unsigned long set_a_size;
  unsigned long set_b_size;
  xt_case_set_t *larger_set;
  xt_case_set_t *smaller_set;

  set_a_size = xt_case_set_get_size(set_a);
  set_b_size = xt_case_set_get_size(set_b);
  if (set_a_size > set_b_size) {
    larger_set = set_a;
    smaller_set = set_b;
  } else {
    larger_set = set_b;
    smaller_set = set_a;
  }

  overlaps = xt_core_bool_false;
  xt_case_set_iterate_start(smaller_set);
  while ((object = xt_case_set_iterate_next(smaller_set))) {
    if (xt_case_set_find(larger_set, object)) {
      overlaps = xt_core_bool_true;
      break;
    }
  }

  return overlaps;
}
