#ifndef xt_file_basic_h
#define xt_file_basic_h

#include "xt/case/list.h"
#include "xt/file/mode.h"

struct xt_file_basic_t;
typedef struct xt_file_basic_t xt_file_basic_t;

xt_file_basic_t *xt_file_basic_create(const char *filename,
    xt_file_mode_t mode);

void xt_file_basic_destroy(xt_file_basic_t *file);

xt_core_bool_t xt_file_basic_exists(const char *filename);

xt_core_bool_t xt_file_basic_get_as_blob(xt_file_basic_t *file,
    char **blob, unsigned long *blob_size);

xt_case_list_t *xt_file_basic_get_as_line_list(xt_file_basic_t *file);

char *xt_file_basic_get_as_string(xt_file_basic_t *file);

xt_case_list_t *xt_file_basic_get_as_token_list(xt_file_basic_t *file,
    char *delimiters);

FILE *xt_file_basic_get_file(xt_file_basic_t *file);

xt_core_bool_t xt_file_basic_get_line_count(xt_file_basic_t *file,
    unsigned long *line_count);

xt_core_bool_t xt_file_basic_write_char(xt_file_basic_t *file, int c);

xt_core_bool_t xt_file_basic_write_string(xt_file_basic_t *file, char *string);

#endif
