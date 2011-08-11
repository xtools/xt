#include "xt/core/standard.h"
#include "xt/core/color.h"

void xt_core_color_init(xt_core_color_t *color, unsigned long red,
    unsigned long blue, unsigned long green)
{
  assert(color);

  color->red = red;
  color->green = green;
  color->blue = blue;
}
