// TODO: Make better example of STF test module

#define STF_IMPL_MOD
#include "../src/stf.h"

uint32_t test(void *arg) {
  printf("%p\n", arg);

  return STF_SUCCESS;
}

uint32_t stf_module_list(void *_module_list)
{
  ModuleList *this = MODULE_LIST_ARGUMENT(_module_list);
  stf_module_list_add(this, TEST(not_implemented_test, "A test with no implementation", NULL));
  stf_module_list_add(this, TEST(test, "test", NULL));

  return STF_SUCCESS;
}