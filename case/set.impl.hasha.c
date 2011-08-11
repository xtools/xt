#include "xt/case/set.h"
#include "xt/case/hasha.h"

xt_core_bool_t xt_case_set_add(xt_case_set_t *set, void *object)
{
  return xt_case_hasha_add((xt_case_hasha_t *) set, object);
}

xt_core_bool_t xt_case_set_add_replace(xt_case_set_t *set, void *object)
{
  return xt_core_bool_false;
}

xt_case_set_t *xt_case_set_create(xt_core_iobject_t *iobject)
{
  return (xt_case_set_t *) xt_case_hasha_create(iobject,
      XT_CASE_HASHA_SET_TYPE_SET);
}

void xt_case_set_destroy(void *set_object)
{
  xt_case_hasha_destroy(set_object);
}

void *xt_case_set_find(xt_case_set_t *set, void *decoy_object)
{
  return xt_case_hasha_find((xt_case_hasha_t *) set, decoy_object);
}

void *xt_case_set_find_any(xt_case_set_t *set)
{
  return NULL;
}

xt_core_iobject_t *xt_case_set_get_iobject(xt_case_set_t *set)
{
  return xt_case_hasha_get_iobject((xt_case_hasha_t *) set);
}

unsigned long xt_case_set_get_size(xt_case_set_t *set)
{
  return xt_case_hasha_get_size((xt_case_hasha_t *) set);
}

void *xt_case_set_iterate_next(xt_case_set_t *set)
{
  return xt_case_hasha_iterate_next((xt_case_hasha_t *) set);
}

void xt_case_set_iterate_remove(xt_case_set_t *set)
{
  xt_case_hasha_iterate_remove((xt_case_hasha_t *) set);
}

void xt_case_set_iterate_start(xt_case_set_t *set)
{
  xt_case_hasha_iterate_start((xt_case_hasha_t *) set);
}

void xt_case_set_print(xt_case_set_t *set,
    xt_core_object_get_as_string_f get_object_as_string)
{
  assert(set);
  assert(get_object_as_string);
  void *object;
  char *string;

  xt_case_set_iterate_start(set);
  while ((object = xt_case_set_iterate_next(set))) {
    string = get_object_as_string(object);
    if (string) {
      printf("%s,", string);
      free(string);
    } else {
      xt_trace("get_object_as_string");
    }
  }
  printf("\n");
}

xt_core_bool_t xt_case_set_remove(xt_case_set_t *set, void *object)
{
  xt_case_hasha_remove((xt_case_hasha_t *) set, object);
  return xt_core_bool_true;
}
