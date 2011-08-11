#include "xt/case/map.h"
#include "xt/case/set.h"
#include "xt/core/pair.h"
#include "xt/core/tools.h"

struct xt_case_map_t {
  xt_case_set_t *pairs;
  xt_core_iobject_t *left_iobject;
  xt_core_iobject_t *right_iobject;
  xt_core_bool_t destroy;
  xt_core_iobject_t pairs_iobject;
};

xt_core_bool_t xt_case_map_add(xt_case_map_t *map, void *left,
    void *right)
{
  assert(map);
  assert(left);
  assert(right);
  xt_core_pair_t *pair;
  xt_core_bool_t success;

  pair = xt_core_pair_create(left, map->left_iobject, right,
      map->right_iobject, map->destroy);
  if (pair) {
    success = xt_core_bool_true;
    xt_case_set_add(map->pairs, pair);
  } else {
    success = xt_core_bool_false;
    xt_core_trace("x_core_pair_create");
  }

  return success;
}

int xt_case_map_compare(void *map_object_a, void *map_object_b)
{
  assert(map_object_a);
  assert(map_object_b);
  xt_case_map_t *map_a;
  xt_case_map_t *map_b;

  map_a = map_object_a;
  map_b = map_object_b;

  return xt_case_set_compare(map_a->pairs, map_b->pairs);
}

void *xt_case_map_copy(void *map_object)
{
  assert(map_object);
  xt_case_map_t *map;
  xt_case_map_t *map_copy;

  map = map_object;

  map_copy = xt_case_map_create(map->left_iobject, map->right_iobject,
      map->destroy);
  if (map_copy) {
    map_copy->pairs = xt_case_set_copy(map->pairs);
    if (!map_copy->pairs) {
      xt_core_trace("x_case_set_copy");
      free(map_copy);
      map_copy = NULL;
    }
  } else {
    xt_core_trace("x_case_map_create");
  }

  return map_copy;
}

xt_case_map_t *xt_case_map_create(xt_core_iobject_t *left_iobject,
    xt_core_iobject_t *right_iobject, xt_core_bool_t destroy)
{
  assert(left_iobject);
  assert(right_iobject);
  xt_case_map_t *map;

  map = malloc(sizeof *map);
  if (map) {
    map->left_iobject = left_iobject;
    map->right_iobject = right_iobject;
    map->destroy = destroy;
    xt_core_iobject_init(&map->pairs_iobject, xt_core_pair_compare_left,
        xt_core_pair_equal_left, xt_core_pair_copy, xt_core_pair_destroy,
        XT_CORE_OBJECT_NO_GET_AS_STRING_F, xt_core_pair_mod_left);
    map->pairs = xt_case_set_create(&map->pairs_iobject);
    if (!map->pairs) {
      xt_core_trace("x_case_set_create");
      free(map);
      map = NULL;
    }
  } else {
    xt_core_trace("malloc");
  }

  return map;
}

void xt_case_map_destroy(void *map_object)
{
  assert(map_object);
  xt_case_map_t *map;

  map = map_object;

  xt_case_set_destroy(map->pairs);
  free(map);
}

void *xt_case_map_find(xt_case_map_t *map,
    void *left_object_decoy)
{
  assert(map);
  assert(left_object_decoy);
  xt_core_pair_t *found_pair;
  void *found_object;
  xt_core_pair_t *pair_decoy;

  pair_decoy = xt_core_pair_create_decoy(left_object_decoy, map->left_iobject);
  if (pair_decoy) {
    found_pair = xt_case_set_find(map->pairs, pair_decoy);
    xt_core_pair_destroy_decoy(pair_decoy);
    if (found_pair) {
      found_object = xt_core_pair_get_right(found_pair);
    } else {
      found_object = NULL;
    }
  } else {
    found_object = NULL;
    xt_core_trace("pair_create_decoy");
  }

  return found_object;
}

void xt_case_map_print(xt_case_map_t *map)
{
  xt_case_set_print(map->pairs, xt_core_pair_get_as_string);
}

xt_core_bool_t xt_case_map_remove(xt_case_map_t *map,
    void *left_object_decoy)
{
  assert(map);
  assert(left_object_decoy);
  xt_core_bool_t success;
  xt_core_pair_t *pair_decoy;

  pair_decoy = xt_core_pair_create_decoy(left_object_decoy, map->left_iobject);
  if (pair_decoy) {
    success = xt_case_set_remove(map->pairs, pair_decoy);
  } else {
    success = xt_core_bool_false;
    xt_core_trace("pair_create_decoy");
  }

  return success;
}
