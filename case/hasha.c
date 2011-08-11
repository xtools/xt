#include "xt/case/hasha.h"

struct xt_case_hasha_t {
};

xt_core_bool_t xt_case_hasha_add(xt_case_hasha_t *hasha, void *object)
{
  xt_core_implement();
  return xt_core_bool_false;
}

void xt_case_hasha_clear(xt_case_hasha_t *hasha)
{
  xt_core_implement();
}

xt_case_hasha_t *xt_case_hasha_create(xt_core_iobject_t *iobject)
{
  xt_core_implement();
  return NULL;
}

void xt_case_hasha_destroy(xt_case_hasha_t *hasha)
{
  xt_core_implement();
}

void xt_case_hasha_dont_destroy_objects(xt_case_hasha_t *hasha)
{
  xt_core_implement();
}

void *xt_case_hasha_find(xt_case_hasha_t *hasha, void *decoy_object)
{
  xt_core_implement();
  return NULL;
}

xt_core_iobject_t *xt_case_hasha_get_iobject(xt_case_hasha_t *hasha)
{
  xt_core_implement();
  return NULL;
}

unsigned long xt_case_hasha_get_size(xt_case_hasha_t *hasha)
{
  xt_core_implement();
  return 0;
}

void xt_case_hasha_iterate_remove(xt_case_hasha_t *hasha)
{
  xt_core_implement();
}

void xt_case_hasha_iterate_start(xt_case_hasha_t *hasha)
{
  xt_core_implement();
}

void *xt_case_hasha_iterate_next(xt_case_hasha_t *hasha)
{
  xt_core_implement();
  return NULL;
}

xt_core_bool_t xt_case_hasha_remove(xt_case_hasha_t *hasha,
    void *decoy_object)
{
  xt_core_implement();
  return xt_core_bool_false;
}
