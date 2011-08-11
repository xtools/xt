#ifndef xt_core_tools_h
#define xt_core_tools_h

#include "xt/core/bit.h"
#include "xt/core/standard.h"

#define XT_CORE_BYTES_IN_A_KILOBYTE 1024
#define XT_CORE_BYTES_IN_A_MEGABYTE 1048576
#define XT_CORE_BYTES_IN_A_GIGABYTE 1073741824

#define XT_CORE_MAX_COLOR 2147483647

#define XT_CORE_SECONDS_PER_MINUTE 60
#define XT_CORE_SECONDS_PER_HOUR 3600
#define XT_CORE_SECONDS_PER_DAY 86400

#define XT_CORE_STANDARD_SLEEP_MICROSECONDS 1024

#define XT_CORE_WHITESPACE_DELIMITERS " \t\n\r"

#define xt_disable_test() printf("  %s disabled\n", argv[0]); return 0

#define xt_core_get_bit(value, bit_index) ((value >> bit_index) & 1)

#define xt_core_hash(string) xt_core_hash_djb2_xor(string)

#define xt_core_implement() xt_core_trace("TODO: implement"); exit(19)

#define xt_core_max(a, b) ((a > b) ? a : b)

#define xt_core_min(a, b) ((a < b) ? a : b)

#define xt_core_trace(message, ...) fprintf(stderr, message "() in %s:%d\n", \
      ##__VA_ARGS__, __FILE__, __LINE__)

#define xt_core_trace_exit(message, ...) fprintf                        \
  (stderr, message"() in %s:%d\n", ##__VA_ARGS__, __FILE__, __LINE__);  \
  exit(22);

#define XT_CORE_TIME_STRING_LENGTH 15
typedef char xt_core_time_string_t[XT_CORE_TIME_STRING_LENGTH + 1];

typedef void *(*xt_core_thread_f)(void *thread_context);

xt_core_bool_t xt_core_stop_requested;

unsigned long xt_get_microseconds();

xt_core_bool_t xt_core_get_current_time_string(xt_core_time_string_t time_string);

void xt_core_do_nothing();

double xt_core_gray_coin_toss();

unsigned long xt_core_hash_djb2(char *string);

unsigned long xt_core_hash_djb2_xor(char *string);

unsigned long xt_core_hash_sdbm(char *string);

xt_core_bool_t xt_core_key_hit();

void xt_core_note_maximum_double(double *maximum, double candidate);

void xt_core_note_maximum_unsigned_long(unsigned long *maximum,
    unsigned long candidate);

double xt_core_percentage(double part, double whole);

void xt_core_request_stop();

void xt_core_seed_random(unsigned long seed);

void xt_core_seed_random_with_time();

void xt_core_set_bit_in_unsigned_char(unsigned char *c,
    unsigned short bit_index, xt_core_bit_t bit_value);

void xt_core_show_memory(void *location, unsigned short length);

char *xt_core_string_append(char *original, const char *addition);

char *xt_core_string_append_char(char *original, char addition);

char *xt_core_string_append_multiple(char *original, ...);

char *xt_core_string_append_n(char *original, char *addition,
    unsigned long addition_size);

char *xt_core_substring(char *string, unsigned short max_length);

void xt_core_terminal_block();

void xt_core_terminal_nonblock();

xt_core_bool_t xt_core_time_is_remaining_microseconds
(struct timeval *start_time, unsigned long max_wall_time_microseconds);

unsigned short xt_core_toss_coin();

void xt_core_truncate_string(char *string, unsigned short max_length);

unsigned long xt_core_wrap_index(long virtual_index, unsigned long range);

#endif
