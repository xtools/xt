#ifndef xt_core_dayhour_h
#define xt_core_dayhour_h

struct xt_core_dayhour_t {
  unsigned short days;
  unsigned short hours;
  unsigned short minutes;
  unsigned short seconds;
};
typedef struct xt_core_dayhour_t xt_core_dayhour_t;

char *create_string_from_dayhour(xt_core_dayhour_t *dayhour);

#endif
