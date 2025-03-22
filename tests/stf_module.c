// TODO: Make better example of STF test module

#define STF_IMPL_MOD
#include "../src/stf.h"

bool test(void *arg)
{
  printf("%p\n", arg);

  return true;
}

bool foo(void *arg)
{
  return (2 + 2) == 4;
}

void stf_module_list(STF_MODULE_LIST)
{
  stf_test_add(MODULE_LIST, TEST(not_implemented_test, "A test with no implementation", NULL));
  stf_test_add(MODULE_LIST, TEST(test, "test", NULL));
  stf_test_add(MODULE_LIST, TEST(foo, "checks if 2 + 2 is 4", NULL));
}