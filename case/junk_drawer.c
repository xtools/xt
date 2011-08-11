#include "xt/case/junk_drawer.h"

/*
  super-simple
  great for small collections
  last-used stuff is near the top
  stochastic
  messy
  that's it
*/

struct xt_case_junk_drawer_t {
};

xt_core_bool_t xt_case_junk_drawer_add
(xt_case_junk_drawer_t *junk_drawer, void *object)
{
  return xt_core_bool_false;
}

void xt_case_junk_drawer_clear(xt_case_junk_drawer_t *junk_drawer)
{
}

xt_case_junk_drawer_t *xt_case_junk_drawer_create
(xt_core_iobject_t *iobject)
{
  return NULL;
}

void xt_case_junk_drawer_destroy(xt_case_junk_drawer_t *junk_drawer)
{
}

void xt_case_junk_drawer_dont_destroy_objects
(xt_case_junk_drawer_t *junk_drawer)
{
}

void *xt_case_junk_drawer_find(xt_case_junk_drawer_t *junk_drawer,
    void *decoy_object)
{
  return NULL;
}

xt_core_iobject_t *xt_case_junk_drawer_get_iobject
(xt_case_junk_drawer_t *junk_drawer)
{
  return NULL;
}

unsigned long xt_case_junk_drawer_get_size
(xt_case_junk_drawer_t *junk_drawer)
{
  return 0;
}

void xt_case_junk_drawer_iterate_remove(xt_case_junk_drawer_t *junk_drawer)
{
}

void xt_case_junk_drawer_iterate_start(xt_case_junk_drawer_t *junk_drawer)
{
}

void *xt_case_junk_drawer_iterate_next(xt_case_junk_drawer_t *junk_drawer)
{
  return NULL;
}

xt_core_bool_t xt_case_junk_drawer_remove
(xt_case_junk_drawer_t *junk_drawer, void *decoy_object)
{
  return xt_core_bool_false;
}
