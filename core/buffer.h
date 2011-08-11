#ifndef xt_core_buffer_h
#define xt_core_buffer_h

#include "xt/core/bool.h"

struct xt_core_buffer_t;
typedef struct xt_core_buffer_t xt_core_buffer_t;

xt_core_bool_t xt_core_buffer_append_char(xt_core_buffer_t *buffer, char c);

xt_core_bool_t xt_core_buffer_append_string(xt_core_buffer_t *buffer,
    char *string, unsigned long string_size);

void xt_core_buffer_clear(xt_core_buffer_t *buffer);

xt_core_buffer_t *xt_core_buffer_create(char *string, unsigned long string_size);

void xt_core_buffer_destroy(xt_core_buffer_t *buffer);

char *xt_core_buffer_get_string(xt_core_buffer_t *buffer,
    unsigned long *string_size);

char *xt_core_buffer_take_string(xt_core_buffer_t *buffer,
    unsigned long *string_size);

#endif
