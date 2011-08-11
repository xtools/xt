#ifndef xt_core_log_h
#define xt_core_log_h

#include "xt/core/bool.h"
#include "xt/core/tools.h"

#define xt_core_log_trace(log, system, entry, ...) _x_audit_log_trace \
  (log, system, entry, __FILE__, __LINE__, ##__VA_ARGS__);

#define xt_core_log_trace_exit(log, system, entry, ...) _x_audit_log_trace \
  (log, system, entry, __FILE__, __LINE__, ##__VA_ARGS__); exit(44);

struct xt_core_log_t;
typedef struct xt_core_log_t xt_core_log_t;

xt_core_bool_t xt_core_log_add_file(xt_core_log_t *log, FILE *file);

xt_core_log_t *xt_core_log_create(FILE *file);

void xt_core_log_destroy(xt_core_log_t *log);

xt_core_bool_t xt_core_log_enter(xt_core_log_t *log, const char *system,
    const char *entry, ...);

xt_core_bool_t xt_core_log_remove_file(xt_core_log_t *log, FILE *file);

xt_core_bool_t _x_audit_log_trace(xt_core_log_t *log, const char *system,
    const char *entry, const char *file, unsigned long line, ...);

#endif
