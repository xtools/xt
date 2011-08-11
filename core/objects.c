#include "xt/core/bool.h"
#include "xt/core/objects.h"
#include "xt/core/pair.h"
#include "xt/core/standard.h"
#include "xt/core/string.h"
#include "xt/core/tools.h"
#include "xt/core/uuid.h"
#include "xt/core/basic/long.h"
#include "xt/core/basic/unsigned_long.h"
#include "xt/core/basic/void.h"

static xt_core_bool_t initialized = xt_core_bool_false;

void xt_core_objects_init()
{
  if (!initialized) {
    xt_core_basic_long_init_iobject(&xt_core_objects.long_iobject);
    xt_core_pair_init_iobject(&xt_core_objects.pair_iobject);
    xt_core_string_init_iobject(&xt_core_objects.string_iobject);
    xt_core_basic_unsigned_long_init_iobject
      (&xt_core_objects.unsigned_long_iobject);
    xt_core_uuid_init_iobject(&xt_core_objects.uuid_iobject);
    xt_core_basic_void_init_iobject(&xt_core_objects.void_iobject);
    initialized = xt_core_bool_true;
  } else {
    xt_core_trace("fyi :: xt_core_objects_init called more than once");
  }
}
