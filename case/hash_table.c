#include "xt/case/hash_table.h"

struct xt_case_hash_table_t {
};

xt_core_bool_t xt_case_hash_table_add(xt_case_hash_table_t *hash_table,
    void *object)
{
  xt_core_implement();
  return xt_core_bool_false;
}

void xt_case_hash_table_clear(xt_case_hash_table_t *hash_table)
{
  xt_core_implement();
}

xt_case_hash_table_t *xt_case_hash_table_create
(xt_core_iobject_t *iobject)
{
  xt_core_implement();
  return NULL;
}

void xt_case_hash_table_destroy(xt_case_hash_table_t *hash_table)
{
  xt_core_implement();
}

void xt_case_hash_table_dont_destroy_objects
(xt_case_hash_table_t *hash_table)
{
  xt_core_implement();
}

void *xt_case_hash_table_find(xt_case_hash_table_t *hash_table,
    void *decoy_object)
{
  xt_core_implement();
  return NULL;
}

xt_core_iobject_t *xt_case_hash_table_get_iobject
(xt_case_hash_table_t *hash_table)
{
  xt_core_implement();
  return NULL;
}

unsigned long xt_case_hash_table_get_size
(xt_case_hash_table_t *hash_table)
{
  xt_core_implement();
  return 0;
}

void xt_case_hash_table_iterate_remove
(xt_case_hash_table_t *hash_table)
{
  xt_core_implement();
}

void xt_case_hash_table_iterate_start(xt_case_hash_table_t *hash_table)
{
  xt_core_implement();
}

void *xt_case_hash_table_iterate_next(xt_case_hash_table_t *hash_table)
{
  xt_core_implement();
  return NULL;
}

xt_core_bool_t xt_case_hash_table_remove
(xt_case_hash_table_t *hash_table, void *decoy_object)
{
  xt_core_implement();
  return xt_core_bool_false;
}
