#ifndef xt_core_scratch_h
#define xt_core_scratch_h

struct xt_core_scratch_t;
typedef struct xt_core_scratch_t xt_core_scratch_t;

xt_core_scratch_t *xt_core_scratch_create();

void xt_core_scratch_destroy(xt_core_scratch_t *xt_core_scratch);

#endif
