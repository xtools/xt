#ifndef xt_file_directory_h
#define xt_file_directory_h

#include "xt/core/standard.h"
#include "xt/core/bool.h"

struct xt_file_directory_t;
typedef struct xt_file_directory_t xt_file_directory_t;

xt_file_directory_t *xt_file_directory_create();

void xt_file_directory_destroy(xt_file_directory_t *xt_file_directory);

xt_core_bool_t xt_file_directory_exists(char *directory_name);

xt_core_bool_t xt_file_directory_make(char *directory_name, mode_t mode);

#endif
