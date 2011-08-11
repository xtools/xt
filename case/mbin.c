#include "mbin.h"

#define BECOME_SIMPLE_DIVISOR 2
#define MAX_OBJECTS_PER_BIN 8

#define PRIMES_COUNT 10
static unsigned long primes[PRIMES_COUNT] = {29, 23, 19, 17, 13, 11, 7, 5, 3,
                                             2};

struct xt_case_mbin_t {
  xt_core_bool_t container;
  void **objects;
  xt_case_mbin_t **bins;
  unsigned long object_count;
  unsigned long contained_object_count;
  unsigned long level;
  unsigned long bin_count;
  unsigned long iterator;
  xt_core_bool_t iterate_remove;
  xt_case_mbin_set_type_t set_type;
  xt_core_iobject_t *iobject;
};

static xt_core_bool_t become_container(xt_case_mbin_t *mbin);
static void become_simple(xt_case_mbin_t *mbin);
static xt_case_mbin_t *create(xt_core_iobject_t *iobject,
    unsigned long level, xt_case_mbin_set_type_t set_type);
static xt_core_bool_t create_bins(xt_case_mbin_t *mbin);
static void destroy_bins(xt_case_mbin_t *mbin);

xt_core_bool_t become_container(xt_case_mbin_t *mbin)
{
  assert(mbin);
  assert(!mbin->bins);
  assert(!mbin->container);
  xt_core_bool_t success = xt_core_bool_true;
  unsigned long i;
  unsigned long remainder;
  void *object;

  if (create_bins(mbin)) {
    assert(mbin->bins);
    for (i = 0; i < mbin->object_count; i++) {
      object = *(mbin->objects + i);
      remainder = mbin->iobject->mod(object, mbin->bin_count);
      if (!xt_case_mbin_add(*(mbin->bins + remainder), object)) {
        xt_core_trace("x_case_mbin_add");
        success = xt_core_bool_false;
        break;
      }
    }
    if (success) {
      mbin->object_count = 0;
      mbin->container = xt_core_bool_true;
    } else {
      destroy_bins(mbin);
      mbin->bins = NULL;
    }
  } else {
    success = xt_core_bool_false;
    xt_core_trace("create_bins");
    assert(!mbin->bins);
    assert(!mbin->container);
  }

  return success;
}

void become_simple(xt_case_mbin_t *mbin)
{
  assert(mbin);
  assert(mbin->bins);
  assert(mbin->container);
  assert(0 == mbin->object_count);
  unsigned long i;
  void *object;

  /*  printf("become_simple()\n");  */

  xt_case_mbin_iterate_start(mbin);
  while ((object = xt_case_mbin_iterate_next(mbin))) {
    assert(object);
    *(mbin->objects + mbin->object_count) = object;
    mbin->object_count++;
    assert(mbin->object_count <= MAX_OBJECTS_PER_BIN);
  }

  for (i = 0; i < mbin->bin_count; i++) {
    xt_case_mbin_dont_destroy_objects(*(mbin->bins + i));
  }
  destroy_bins(mbin);
  mbin->bins = NULL;
  mbin->container = xt_core_bool_false;
}

xt_case_mbin_t *create(xt_core_iobject_t *iobject, unsigned long level,
    xt_case_mbin_set_type_t set_type)
{
  assert(iobject);
  assert(iobject->mod);
  assert(iobject->compare_equal);
  assert(level < PRIMES_COUNT);
  xt_case_mbin_t *mbin;

  mbin = malloc(sizeof *mbin);
  if (mbin) {
    mbin->container = xt_core_bool_false;
    mbin->bins = NULL;
    mbin->object_count = 0;
    mbin->contained_object_count = 0;
    mbin->iobject = iobject;
    mbin->level = level;
    mbin->bin_count = primes[level];
    mbin->set_type = set_type;
    mbin->objects = malloc((sizeof *mbin->objects) * MAX_OBJECTS_PER_BIN);
    if (!mbin->objects) {
      xt_core_trace("malloc");
    }
  } else {
    xt_core_trace("malloc");
  }

  return mbin;
}

xt_core_bool_t create_bins(xt_case_mbin_t *mbin)
{
  assert(mbin);
  assert(!mbin->bins);
  xt_core_bool_t success;
  unsigned long i;

  mbin->bins = malloc((sizeof *mbin->bins) * mbin->bin_count);
  if (mbin->bins) {
    success = xt_core_bool_true;
    for (i = 0; i < mbin->bin_count; i++) {
      if (mbin->level < (PRIMES_COUNT - 1)) {
        *(mbin->bins + i) = create(mbin->iobject, mbin->level + 1,
            mbin->set_type);
      } else {
        *(mbin->bins + i)
          = create(mbin->iobject, mbin->level, mbin->set_type);
        xt_core_trace("ran out of primes");
      }
      if (!*(mbin->bins + i)) {
        xt_core_trace("create");
        success = xt_core_bool_false;
        break;
      }
    }
  } else {
    xt_core_trace("malloc");
    success = xt_core_bool_false;
  }

  return success;
}

void destroy_bins(xt_case_mbin_t *mbin)
{
  assert(mbin);
  assert(mbin->bins);
  unsigned long i;

  for (i = 0; i < mbin->bin_count; i++) {
    xt_case_mbin_destroy(*(mbin->bins + i));
  }
  free(mbin->bins);
}

xt_core_bool_t xt_case_mbin_add(xt_case_mbin_t *mbin, void *object)
{
  assert(mbin);
  assert(object);
  assert(mbin->object_count <= MAX_OBJECTS_PER_BIN);
  xt_core_bool_t success = xt_core_bool_true;
  unsigned long remainder;
  xt_case_mbin_t *bin;

  if (mbin->object_count == MAX_OBJECTS_PER_BIN) {
    if (!become_container(mbin)) {
      success = xt_core_bool_false;
      xt_core_trace("become_container");
    }
  }

  if (success) {
    if (mbin->container) {
      remainder = mbin->iobject->mod(object, mbin->bin_count);
      bin = *(mbin->bins + remainder);

      if (XT_CASE_MBIN_SET_TYPE_MULTISET == mbin->set_type) {
        if (xt_case_mbin_add(bin, object)) {
          mbin->contained_object_count++;
        } else {
          success = xt_core_bool_false;
          xt_core_trace("x_case_mbin_add");
        }
      } else if (XT_CASE_MBIN_SET_TYPE_SET == mbin->set_type) {
        if (xt_case_mbin_add(bin, object)) {
          mbin->contained_object_count++;
        } else {
          success = xt_core_bool_false;
        }
      }

    } else {

      if (XT_CASE_MBIN_SET_TYPE_MULTISET == mbin->set_type) {
        assert(mbin->object_count < MAX_OBJECTS_PER_BIN);
        *(mbin->objects + mbin->object_count) = object;
        mbin->object_count++;
        mbin->contained_object_count++;
      } else if (XT_CASE_MBIN_SET_TYPE_SET == mbin->set_type) {
        if (xt_case_mbin_find(mbin, object)) {
          success = xt_core_bool_false;
        } else {
          assert(mbin->object_count < MAX_OBJECTS_PER_BIN);
          *(mbin->objects + mbin->object_count) = object;
          mbin->object_count++;
          mbin->contained_object_count++;
        }
      }

    }
  }

  return success;
}

void xt_case_mbin_clear(xt_case_mbin_t *mbin)
{
  assert(mbin);

  xt_case_mbin_iterate_start(mbin);
  while (xt_case_mbin_iterate_next(mbin)) {
    xt_case_mbin_iterate_remove(mbin);
  }
}

xt_case_mbin_t *xt_case_mbin_create(xt_core_iobject_t *iobject,
    xt_case_mbin_set_type_t set_type)
{
  return create(iobject, 0, set_type);
}

void xt_case_mbin_destroy(xt_case_mbin_t *mbin)
{
  assert(mbin);
  unsigned long i;

  if (mbin->container) {
    destroy_bins(mbin);
  } else {
    if (mbin->iobject->destroy) {
      for (i = 0; i < mbin->object_count; i++) {
        mbin->iobject->destroy(*(mbin->objects + i));
      }
    }
  }
  free(mbin->objects);
  free(mbin);
}

void xt_case_mbin_dont_destroy_objects(xt_case_mbin_t *mbin)
{
  assert(mbin);
  unsigned long i;

  mbin->iobject->destroy = NULL;
  if (mbin->container) {
    for (i = 0; i < mbin->bin_count; i++) {
      xt_case_mbin_dont_destroy_objects(*(mbin->bins + i));
    }
  }
}

void *xt_case_mbin_find(xt_case_mbin_t *mbin, void *decoy_object)
{
  assert(mbin);
  assert(decoy_object);
  unsigned long i;
  unsigned long remainder;
  void *object = NULL;

  /*  printf("find()\n");  */

  if (mbin->container) {
    remainder = mbin->iobject->mod(decoy_object, mbin->bin_count);
    object = xt_case_mbin_find(*(mbin->bins + remainder), decoy_object);
  } else {
    for (i = 0; i < mbin->object_count; i++) {
      if (mbin->iobject->compare_equal(decoy_object, *(mbin->objects + i))) {
        object = *(mbin->objects + i);
        break;
      }
    }
  }

  return object;
}

xt_core_iobject_t *xt_case_mbin_get_iobject(xt_case_mbin_t *mbin)
{
  assert(mbin);
  return mbin->iobject;
}

unsigned long xt_case_mbin_get_size(xt_case_mbin_t *mbin)
{
  assert(mbin);
  return mbin->contained_object_count;
}

void xt_case_mbin_iterate_remove(xt_case_mbin_t *mbin)
{
  assert(mbin);
  mbin->iterate_remove = xt_core_bool_true;
}

void xt_case_mbin_iterate_start(xt_case_mbin_t *mbin)
{
  assert(mbin);

  mbin->iterator = 0;
  mbin->iterate_remove = xt_core_bool_false;
  if (mbin->container) {
    xt_case_mbin_iterate_start(*(mbin->bins));
  }
}

void *xt_case_mbin_iterate_next(xt_case_mbin_t *mbin)
{
  assert(mbin);
  void *object;

  if (mbin->container) {
    if (mbin->iterator < mbin->bin_count) {
      if (mbin->iterate_remove) {
        xt_case_mbin_iterate_remove(*(mbin->bins + mbin->iterator));
        if ((*(mbin->bins + mbin->iterator))->contained_object_count > 0) {
          mbin->contained_object_count--;
        }
      }
      object = xt_case_mbin_iterate_next(*(mbin->bins + mbin->iterator));
      while (!object && (mbin->iterator < (mbin->bin_count - 1))) {
        mbin->iterator++;
        xt_case_mbin_iterate_start(*(mbin->bins + mbin->iterator));
        object = xt_case_mbin_iterate_next(*(mbin->bins + mbin->iterator));
      }
    } else {
      object = NULL;
    }
  } else {
    if (mbin->iterate_remove) {
      assert(mbin->iterator > 0);
      assert(mbin->object_count > 0);
      assert(mbin->iterator <= mbin->object_count);
      if (mbin->iobject->destroy) {
        mbin->iobject->destroy(*(mbin->objects + (mbin->iterator - 1)));
      }
      if (mbin->object_count > 1) {
        *(mbin->objects + (mbin->iterator - 1))
          = *(mbin->objects + mbin->object_count - 1);
      } else {
        *(mbin->objects + (mbin->iterator - 1)) = NULL;
      }
      mbin->object_count--;
      mbin->contained_object_count--;
      mbin->iterator--;
      mbin->iterate_remove = xt_core_bool_false;
    }
    if (mbin->iterator < mbin->object_count) {
      object = *(mbin->objects + mbin->iterator);
      mbin->iterator++;
    } else {
      object = NULL;
    }
  }

  return object;
}

xt_core_bool_t xt_case_mbin_remove(xt_case_mbin_t *mbin,
    void *decoy_object)
{
  assert(mbin);
  assert(decoy_object);
  unsigned long remainder;
  xt_core_bool_t success = xt_core_bool_false;
  unsigned long i;

  if (mbin->container) {
    remainder = mbin->iobject->mod(decoy_object, mbin->bin_count);
    if (xt_case_mbin_remove(*(mbin->bins + remainder), decoy_object)) {
      success = xt_core_bool_true;
      mbin->contained_object_count--;
      if (mbin->contained_object_count
          < (MAX_OBJECTS_PER_BIN / BECOME_SIMPLE_DIVISOR)) {
        become_simple(mbin);
      }
    }
  } else {
    for (i = 0; i < mbin->object_count; i++) {
      if (mbin->iobject->compare_equal(decoy_object, *(mbin->objects + i))) {
        success = xt_core_bool_true;
        if (mbin->iobject->destroy) {
          mbin->iobject->destroy(*(mbin->objects + i));
        }
        if (mbin->object_count > 0) {
          *(mbin->objects + i) = *(mbin->objects + (mbin->object_count - 1));
          mbin->object_count--;
          mbin->contained_object_count--;
        }
        break;
      }
    }
  }

  return success;
}
