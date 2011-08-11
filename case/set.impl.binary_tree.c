#include "xt/case/set.h"
#include "xt/core/string.h"
#include "xt/core/tools.h"
#include "xt/core/basic/long.h"

#define NO_LEFT_OBJECT NULL
#define NO_RIGHT_OBJECT NULL
#define NO_PARENT_OBJECT NULL

struct set_object_t;
typedef struct set_object_t set_object_t;

struct set_object_t {
  void *object;
  set_object_t *parent;
  set_object_t *left;
  set_object_t *right;
};

struct xt_case_set_t {
  xt_core_object_get_as_string_f get_object_as_string;
  unsigned long size;
  xt_core_bool_t iterate_remove;
  xt_core_bool_t iterate_first;
  set_object_t *iterator;

  set_object_t *base;

  pthread_mutex_t mutex;

  xt_core_iobject_t *iobject;
};

/*
  TODO: are these statics? what are these?
*/
set_object_t *_x_case_set_find_first(xt_case_set_t *set);

set_object_t *_x_case_set_find_next(xt_case_set_t *set,
    set_object_t *set_object);

void _x_case_set_remove_set_object(xt_case_set_t *set,
    set_object_t *set_object);

static void assign_to_child(set_object_t *parent, set_object_t *child,
    set_object_t *new_value);

static set_object_t *find_first_parent_greater_than_me(xt_case_set_t *set,
    set_object_t *set_object);

static set_object_t *find_in_order_predecessor(xt_case_set_t *set,
    set_object_t *set_object);

static set_object_t *find_in_order_successor(xt_case_set_t *set,
    set_object_t *set_object);

static set_object_t *find_set_object_containing(xt_case_set_t *set,
    set_object_t *base_set_object, void *object);

static void print(xt_case_set_t *set, set_object_t *base_set_object);

static set_object_t *put_object(xt_case_set_t *set,
    set_object_t *base_set_object, void *object, set_object_t *parent);

static void remove_set_object_both_children(xt_case_set_t *set,
    set_object_t *set_object);
static void remove_set_object_left_child_only(xt_case_set_t *set,
    set_object_t *set_object);
static void remove_set_object_no_children(xt_case_set_t *set,
    set_object_t *set_object);
static void remove_set_object_right_child_only(xt_case_set_t *set,
    set_object_t *set_object);

static set_object_t *set_object_create(void *object,
    set_object_t *parent, set_object_t *left, set_object_t *right);

static void set_object_destroy(xt_case_set_t *set,
    set_object_t *set_object);

set_object_t *_x_case_set_find_first(xt_case_set_t *set)
{
  set_object_t *first;

  first = set->base;
  if (first && first->left) {
    while (first->left) {
      first = first->left;
    }
  }

  return first;
}

set_object_t *_x_case_set_find_next(xt_case_set_t *set,
    set_object_t *set_object)
{
  assert(set_object);
  set_object_t *successor;

  successor = set_object->right;
  if (successor) {
    while (successor->left) {
      successor = successor->left;
    }
  } else {
    if (set_object->parent) {
      if (set_object == set_object->parent->left) {
        successor = set_object->parent;
      } else {
        successor = find_first_parent_greater_than_me(set, set_object);
      }

    } else {
      successor = NULL;
    }
  }

  return successor;
}

void _x_case_set_remove_set_object(xt_case_set_t *set,
    set_object_t *set_object)
{
  assert(set);
  assert(set_object);

  if (!set_object->left && !set_object->right) {
    remove_set_object_no_children(set, set_object);

  } else if (set_object->left && !set_object->right) {
    remove_set_object_left_child_only(set, set_object);

  } else if (!set_object->left && set_object->right) {
    remove_set_object_right_child_only(set, set_object);

  } else {
    remove_set_object_both_children(set, set_object);

  }

  set->size--;
}

void assign_to_child(set_object_t *parent, set_object_t *child,
    set_object_t *new_value)
{
  assert(parent);

  if (child == parent->left) {
    parent->left = new_value;
  } else if (child == parent->right) {
    parent->right = new_value;
  }
  if (new_value) {
    new_value->parent = parent;
  }
}

set_object_t *find_first_parent_greater_than_me(xt_case_set_t *set,
    set_object_t *set_object)
{
  assert(set);
  assert(set_object);
  int compare;
  set_object_t *parent;

  parent = set_object->parent;
  if (parent) {
    compare = set->iobject->compare(parent->object,
        set_object->object);
    while (compare < 0) {
      parent = parent->parent;
      if (parent) {
        compare = set->iobject->compare(parent->object,
            set_object->object);
      } else {
        break;
      }
    }
  }

  return parent;
}

set_object_t *find_in_order_predecessor(xt_case_set_t *set,
    set_object_t *set_object)
{
  assert(set_object);
  set_object_t *predecessor;

  predecessor = set_object->left;
  if (predecessor) {
    while (predecessor->right) {
      predecessor = predecessor->right;
    }
  } else {
    predecessor = NULL;
  }

  return predecessor;
}

set_object_t *find_in_order_successor(xt_case_set_t *set,
    set_object_t *set_object)
{
  assert(set_object);
  set_object_t *successor;

  successor = set_object->right;
  if (successor) {
    while (successor->left) {
      successor = successor->left;
    }
  } else {
    successor = NULL;
  }

  return successor;
}

set_object_t *find_set_object_containing(xt_case_set_t *set,
    set_object_t *base_set_object, void *object)
{
  int compare;
  set_object_t *containing_object;

  if (base_set_object) {
    compare = set->iobject->compare(object, base_set_object->object);
    if (compare < 0) {
      containing_object = find_set_object_containing(set,
          base_set_object->left, object);
    } else if (compare > 0) {
      containing_object = find_set_object_containing(set,
          base_set_object->right, object);
    } else {
      containing_object = base_set_object;
    }
  } else {
    containing_object = NULL;
  }

  return containing_object;
}

xt_core_bool_t xt_case_set_add(xt_case_set_t *set, void *object)
{
  assert(set);
  assert(object);
  set_object_t *set_object;
  xt_core_bool_t success;

  set_object = put_object(set, set->base, object, NO_PARENT_OBJECT);
  if (set_object) {
    success = xt_core_bool_true;
    if (!set->base) {
      set->base = set_object;
    }
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_case_set_add_replace(xt_case_set_t *set, void *object)
{
  xt_core_bool_t success;

  if (xt_case_set_find(set, object)) {
    xt_case_set_remove(set, object);
  }
  success = xt_case_set_add(set, object);

  return success;
}

xt_case_set_t *xt_case_set_create(xt_core_iobject_t *iobject)
{
  assert(iobject);
  assert(iobject->compare);
  xt_case_set_t *set;

  set = malloc(sizeof *set);
  if (set) {
    set->iobject = iobject;
    set->get_object_as_string = NULL;
    set->base = NULL;
    set->size = 0;
    if (0 != pthread_mutex_init(&set->mutex, NULL)) {
      xt_core_trace("pthread_mutex_init");
    }
  }

  return set;
}

void xt_case_set_destroy(void *set_object)
{
  assert(set_object);
  xt_case_set_t *set;

  set = set_object;

  xt_case_set_clear(set);
  if (0 != pthread_mutex_destroy(&set->mutex)) {
    xt_core_trace("pthread_mutex_destroy");
  }
  free(set);
}

void *xt_case_set_find(xt_case_set_t *set, void *decoy_object)
{
  set_object_t *set_object;
  void *found_object;

  set_object = find_set_object_containing(set, set->base, decoy_object);
  if (set_object) {
    found_object = set_object->object;
  } else {
    found_object = NULL;
  }

  return found_object;
}

void *xt_case_set_find_any(xt_case_set_t *set)
{
  assert(set);
  void *any_object;

  if (set->base) {
    any_object = set->base->object;
  } else {
    any_object = NULL;
  }

  return any_object;
}

xt_core_iobject_t *xt_case_set_get_iobject(xt_case_set_t *set)
{
  return set->iobject;
}

unsigned long xt_case_set_get_size(xt_case_set_t *set)
{
  return set->size;
}

void *xt_case_set_iterate_next(xt_case_set_t *set)
{
  assert(set);
  void *next_object;
  set_object_t *successor;

  if (set->iterator) {
    if (set->iterate_first) {
      next_object = set->iterator->object;
      set->iterate_first = xt_core_bool_false;
    } else {
      if (set->iterate_remove) {
        successor = _x_case_set_find_next(set, set->iterator);
        _x_case_set_remove_set_object(set, set->iterator);
        set->iterator = successor;
        set->iterate_remove = xt_core_bool_false;
      } else {
        set->iterator = _x_case_set_find_next(set, set->iterator);
      }
      if (set->iterator) {
        next_object = set->iterator->object;
      } else {
        next_object = NULL;
      }
    }
  } else {
    next_object = NULL;
  }

  return next_object;
}

void xt_case_set_iterate_remove(xt_case_set_t *set)
{
  set->iterate_remove = xt_core_bool_true;
}

void xt_case_set_iterate_start(xt_case_set_t *set)
{
  assert(set);

  set->iterator = _x_case_set_find_first(set);
  set->iterate_remove = xt_core_bool_false;
  set->iterate_first = xt_core_bool_true;
}

void xt_case_set_lock(xt_case_set_t *set)
{
  pthread_mutex_lock(&set->mutex);
}

void xt_case_set_print(xt_case_set_t *set,
    xt_core_object_get_as_string_f get_object_as_string)
{
  set->get_object_as_string = get_object_as_string;
  printf("set (%lu items) : ", set->size);
  print(set, set->base);
  printf("\n");
}

xt_core_bool_t xt_case_set_remove(xt_case_set_t *set, void *object)
{
  xt_core_bool_t success;
  set_object_t *set_object;

  set_object = find_set_object_containing(set, set->base, object);
  if (set_object) {
    _x_case_set_remove_set_object(set, set_object);
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

void xt_case_set_unlock(xt_case_set_t *set)
{
  pthread_mutex_unlock(&set->mutex);
}

void print(xt_case_set_t *set, set_object_t *base_set_object)
{
  char *object_string;

  if (base_set_object) {
    object_string = set->get_object_as_string(base_set_object->object);
    if (object_string) {
      printf("%s", object_string);
      free(object_string);
    } else {
      xt_core_trace("get_as_string");
    }
    printf("(");
    print(set, base_set_object->left);
    printf(",");
    print(set, base_set_object->right);
    printf(")");
  }
}

set_object_t *put_object(xt_case_set_t *set, set_object_t *base_set_object,
    void *object, set_object_t *parent)
{
  int compare;
  set_object_t *new_set_object;

  if (!base_set_object) {
    new_set_object = set_object_create(object, parent, NO_LEFT_OBJECT,
        NO_RIGHT_OBJECT);
    if (new_set_object) {
      set->size++;
    } else {
      xt_core_trace("set_object_create");
    }
  } else {
    compare = set->iobject->compare(object, base_set_object->object);
    if (compare < 0) {
      new_set_object = put_object(set, base_set_object->left, object,
          base_set_object);
      if (!base_set_object->left) {
        base_set_object->left = new_set_object;
      }
    } else if (compare > 0) {
      new_set_object = put_object(set, base_set_object->right, object,
          base_set_object);
      if (!base_set_object->right) {
        base_set_object->right = new_set_object;
      }
    } else {
      new_set_object = NULL;
    }
  }

  return new_set_object;
}

void remove_set_object_both_children(xt_case_set_t *set,
    set_object_t *set_object)
{
  set_object_t *set_object_2;

  if (0 == (random() % 2)) {

    set_object_2 = find_in_order_predecessor(set, set_object);

    if (set_object_2->parent->right == set_object_2) {
      assign_to_child(set_object_2->parent, set_object_2->parent->right,
          set_object_2->left);
    }

    if (set_object->left != set_object_2) {
      set_object_2->left = set_object->left;
      if (set_object->left) {
        set_object->left->parent = set_object_2;
      }
    }

    set_object_2->right = set_object->right;
    if (set_object->right) {
      set_object->right->parent = set_object_2;
    }

    if (set_object->parent) {
      assign_to_child(set_object->parent, set_object, set_object_2);
    } else {
      set->base = set_object_2;
      set_object_2->parent = NULL;
    }

  } else {

    set_object_2 = find_in_order_successor(set, set_object);

    if (set_object_2->parent->left == set_object_2) {
      assign_to_child(set_object_2->parent, set_object_2->parent->left,
          set_object_2->right);
    }

    if (set_object->right != set_object_2) {
      set_object_2->right = set_object->right;
      if (set_object->right) {
        set_object->right->parent = set_object_2;
      }
    }

    set_object_2->left = set_object->left;
    if (set_object->left) {
      set_object->left->parent = set_object_2;
    }

    if (set_object->parent) {
      assign_to_child(set_object->parent, set_object, set_object_2);
    } else {
      set->base = set_object_2;
      set_object_2->parent = NULL;
    }

  }

  set_object_destroy(set, set_object);
}

void remove_set_object_left_child_only(xt_case_set_t *set,
    set_object_t *set_object)
{
  if (set_object->parent) {
    assign_to_child(set_object->parent, set_object, set_object->left);
  } else {
    set->base = set_object->left;
    set_object->left->parent = NULL;
  }
  set_object_destroy(set, set_object);
}

void remove_set_object_no_children(xt_case_set_t *set,
    set_object_t *set_object)
{
  if (set_object->parent) {
    assign_to_child(set_object->parent, set_object, NULL);
  } else {
    set->base = NULL;
  }
  set_object_destroy(set, set_object);
}

void remove_set_object_right_child_only(xt_case_set_t *set,
    set_object_t *set_object)
{
  if (set_object->parent) {
    assign_to_child(set_object->parent, set_object, set_object->right);
  } else {
    set->base = set_object->right;
    set_object->right->parent = NULL;
  }
  set_object_destroy(set, set_object);
}

set_object_t *set_object_create(void *object, set_object_t *parent,
    set_object_t *left, set_object_t *right)
{
  set_object_t *set_object;

  set_object = malloc(sizeof *set_object);
  if (set_object) {
    set_object->object = object;
    set_object->parent = parent;
    set_object->left = left;
    set_object->right = right;
  } else {
    xt_core_trace("malloc");
  }

  return set_object;
}

void set_object_destroy(xt_case_set_t *set, set_object_t *set_object)
{
  assert(set);
  assert(set_object);

  if (set_object->object && set->iobject->destroy) {
    set->iobject->destroy(set_object->object);
  }
  free(set_object);
}
