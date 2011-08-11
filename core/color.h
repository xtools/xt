#ifndef xt_core_color_h
#define xt_core_color_h

struct xt_core_color_t {
  unsigned long red;
  unsigned long blue;
  unsigned long green;
};
typedef struct xt_core_color_t xt_core_color_t;

void xt_core_color_init(xt_core_color_t *color, unsigned long red,
    unsigned long blue, unsigned long green);

#endif
