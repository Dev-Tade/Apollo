// TODO: Make better example of STF test module

#define STF_IMPL_MOD
#include "../src/stf.h"

bool test()
{
  return (2 + 2) == 4;
}

void stf_module_list(stf_Tests *tests)
{
  stf_register_test(tests, STF_TEST_CASE(not_implemented_test, "A test with no implementation"));
  stf_register_test(tests, STF_TEST_CASE(test, "checks if 2 + 2 is 4"));
}