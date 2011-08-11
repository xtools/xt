#include "xt/core/buffer.h"
#include "xt/core/tools.h"

static char *initial_buffer = "happy";
static char *additional_buffer_0 = " day for fun on the beach";
static char *additional_buffer_1 = " with a white paper rabbyt";

int main(int argc, char *argv[])
{
  xt_core_buffer_t *buffer;
  char *string;
  unsigned long string_size;

  buffer = xt_core_buffer_create(initial_buffer, strlen(initial_buffer));
  if (!buffer) {
    xt_core_trace_exit("x_core_buffer_create");
  }

  if (!xt_core_buffer_append_string(buffer, additional_buffer_0,
          strlen(additional_buffer_0))) {
    xt_core_trace_exit("x_core_buffer_append_string");
  }
  if (!xt_core_buffer_append_string(buffer, additional_buffer_1,
          strlen(additional_buffer_1))) {
    xt_core_trace_exit("x_core_buffer_append_string");
  }

  string = xt_core_buffer_get_string(buffer, &string_size);
  printf("string_size:%lu\n", string_size);
  printf(":%s:\n", string);

  xt_core_buffer_destroy(buffer);

  return 0;
}
