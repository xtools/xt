#include "xt/core/standard.h"
#include "xt/case/list.h"
#include "xt/core/string.h"

int main(int argc, char *argv[])
{
  xt_case_list_t *list;
  void *object;
  char *object_string;
  unsigned long index;
  unsigned long iteration;
  xt_case_list_t *strings;
  char *string;
  char *string_to_split;

  list = xt_case_list_create(xt_core_string_compare, xt_core_string_copy,
      xt_core_string_destroy);

  xt_case_list_add_first(list, strdup("puppy"));
  xt_case_list_add_first(list, strdup("rooster"));
  xt_case_list_add_first(list, strdup("cow"));
  xt_case_list_add_last(list, strdup("spots"));

  for (index = 0; index < 5; index++) {
    object = xt_case_list_find_at(list, index);
    if (object) {
      object_string = (char *) object;
      printf("%s\n", object_string);
    }
  }

  printf("\n");

  for (iteration = 0; iteration < 16; iteration++) {
    object = xt_case_list_find_random(list);
    if (object) {
      object_string = (char *) object;
      printf("%s\n", object_string);
    }
  }

  printf("\n");

  xt_case_list_iterate_start(list);
  while ((object = xt_case_list_iterate_next(list))) {
    object_string = (char *) object;
    printf("%s\n", object_string);
  }

  printf("\n");

  xt_case_list_iterate_start(list);
  while ((object = xt_case_list_iterate_next(list))) {
    object_string = (char *) object;
    printf("%s\n", object_string);
  }

  xt_case_list_destroy(list);

  xt_case_list_t *list2;
  list2 = xt_case_list_create(xt_core_string_compare, xt_core_string_copy,
      xt_core_string_destroy);
  xt_case_list_add_first(list2, malloc(7));
  xt_case_list_remove_first(list2);
  xt_case_list_destroy(list2);

  string_to_split = malloc(sizeof("here I am"));
  strcpy(string_to_split, "here I am");

  strings = xt_case_list_create_strings_from_string(string_to_split, " ");
  xt_case_list_iterate_start(strings);
  while ((string = xt_case_list_iterate_next(strings))) {
    printf(":%s:\n", string);
  }
  xt_case_list_destroy(strings);

  free(string_to_split);

  return 0;
}
