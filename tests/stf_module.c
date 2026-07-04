// TODO: Make better example of STF test module

#define STF_IMPL_MOD
#include "../src/stf.h"

bool test();

stf_Test stf_module_tests[] =
{
  {"test", "some test", test},
  {"invalid", "this test function can't be resolved", NULL},
};

STF_MODULE_EXPORTS(stf_module_tests);

bool test()
{
  return (2 + 2) == 4;
}