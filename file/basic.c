#include "xt/core/string.h"
#include "xt/core/tools.h"
#include "xt/file/basic.h"

#define XT_FILE_BASIC_MEMORY_BLOCK_SIZE 4096

static const char *filemode_strings[] = {"r", "w", "a", "r+", "w+", "a+"};

static xt_core_bool_t slurp(xt_file_basic_t *file, char **blob,
    unsigned long *blob_size);

static char *slurp_string(xt_file_basic_t *file);

struct xt_file_basic_t {
  FILE *file;
  xt_case_list_t *token_list_cache;
};

xt_file_basic_t *xt_file_basic_create
(const char *filename, xt_file_mode_t mode)
{
  assert(filename);
  xt_file_basic_t *file;
  const char *mode_string;

  file = malloc(sizeof *file);
  if (file) {
    mode_string = filemode_strings[mode];
    file->token_list_cache = NULL;
    file->file = fopen(filename, mode_string);
    if (!file->file) {
      xt_core_trace("fopen--%s", filename);
    }
  }

  return file;
}

void xt_file_basic_destroy(xt_file_basic_t *file)
{
  assert(file);

  if (file->token_list_cache) {
    xt_case_list_destroy(file->token_list_cache);
  }

  fclose(file->file);
  free(file);

}

xt_core_bool_t xt_file_basic_exists(const char *filename)
{
  assert(filename);
  xt_core_bool_t exists;
  FILE *file;

  file = fopen(filename, filemode_strings[XT_FILE_MODE_OPEN_FOR_READ]);
  if (file) {
    fclose(file);
    exists = xt_core_bool_true;
  } else {
    exists = xt_core_bool_false;
  }

  return exists;
}

xt_core_bool_t xt_file_basic_get_as_blob(xt_file_basic_t *file, char **blob,
    unsigned long *blob_size)
{
  return slurp(file, blob, blob_size);
}

xt_case_list_t *xt_file_basic_get_as_line_list(xt_file_basic_t *file)
{
  return xt_file_basic_get_as_token_list(file, "\n");
}

char *xt_file_basic_get_as_string(xt_file_basic_t *file)
{
  return slurp_string(file);
}

xt_case_list_t *xt_file_basic_get_as_token_list(xt_file_basic_t *file,
    char *delimiters)
{
  assert(file);
  assert(delimiters);
  xt_case_list_t *token_list;
  char *token;
  char *token_copy;
  char *file_string;
  char *strtok_context;

  if (!file->token_list_cache) {
    file->token_list_cache = xt_case_list_create(xt_core_string_compare,
        xt_core_string_copy, xt_core_string_destroy);
    if (file->token_list_cache) {
      file_string = slurp_string(file);
      token = strtok_r(file_string, delimiters, &strtok_context);
      while (token) {
        token_copy = strdup(token);
        if (token_copy) {
          if (!xt_case_list_add_last(file->token_list_cache, token_copy)) {
            xt_core_trace("x_case_list_add_last");
            free(token_copy);
          }
        } else {
          xt_core_trace("strdup");
        }
        token = strtok_r(NULL, delimiters, &strtok_context);
      }
      free(file_string);
    } else {
      xt_core_trace("x_case_list_create");
    }
  }

  if (file->token_list_cache) {
    token_list = xt_case_list_copy(file->token_list_cache);
    if (!token_list) {
      xt_core_trace("x_case_list_copy");
    }
  } else {
    token_list = NULL;
  }

  return token_list;
}

FILE *xt_file_basic_get_file(xt_file_basic_t *file)
{
  return file->file;
}

xt_core_bool_t xt_file_basic_get_line_count(xt_file_basic_t *file,
    unsigned long *line_count)
{
  assert(file);
  xt_core_bool_t success;
  int c;

  if (fseek(file->file, 0, SEEK_SET) == 0) {
    success = xt_core_bool_true;
    *line_count = 0;
    while ((c = fgetc(file->file)) != EOF) {
      if ('\n' == c) {
        (*line_count)++;
      }
    }
  } else {
    success = xt_core_bool_false;
    xt_core_trace("");
  }

  return success;
}

xt_core_bool_t xt_file_basic_write_char(xt_file_basic_t *file, int c)
{
  assert(file);
  xt_core_bool_t success;

  if (EOF != fputc(c, file->file)) {
    success = xt_core_bool_true;
  } else {
    success = xt_core_bool_false;
  }

  return success;
}

xt_core_bool_t xt_file_basic_write_string(xt_file_basic_t *file, char *string)
{
  assert(file);
  xt_core_bool_t success;
  char *c;

  success = xt_core_bool_true;

  c = string;
  while (*c) {
    if (EOF != fputc(*c, file->file)) {
      c++;
    } else {
      success = xt_core_bool_false;
      break;
    }
  }

  return success;
}

/*
  TODO: fix the realloc calls _ they don't check the return value properly
*/
xt_core_bool_t slurp(xt_file_basic_t *file, char **blob,
    unsigned long *blob_size)
{
  assert(file);
  assert(blob);
  assert(blob_size);
  unsigned long characters_read;
  int c;
  xt_core_bool_t success;

  *blob_size = 0;
  characters_read = 0;
  *blob = NULL;
  success = xt_core_bool_true;

  if (fseek(file->file, 0, SEEK_SET) == 0) {
    while ((c = fgetc(file->file)) != EOF) {
      characters_read++;
      if (characters_read > *blob_size) {
        *blob = realloc(*blob, *blob_size + XT_FILE_BASIC_MEMORY_BLOCK_SIZE);
        if (*blob) {
          *blob_size += XT_FILE_BASIC_MEMORY_BLOCK_SIZE;
        } else {
          success = xt_core_bool_false;
          xt_core_trace("realloc");
        }
      }
      if (*blob) {
        *(*blob + (characters_read - 1)) = c;
      }
    }
    *blob = realloc(*blob, characters_read);
    if (!*blob) {
      xt_core_trace("realloc");
    }
    *blob_size = characters_read;
  } else {
    success = xt_core_bool_false;
    xt_core_trace("fseek");
  }

  return success;
}

char *slurp_string(xt_file_basic_t *file)
{
  assert(file);
  char *string;
  char *blob;
  unsigned long blob_size;

  if (slurp(file, &blob, &blob_size)) {
    string = malloc(blob_size + 1);
    if (string) {
      memcpy(string, blob, blob_size);
      *(string + blob_size) = '\0';
    } else {
      xt_core_trace("malloc");
    }
    free(blob);
  } else {
    string = NULL;
    xt_core_trace("malloc");
  }

  return string;
}
