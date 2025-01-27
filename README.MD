# Apollo
A C codebase that implements different data structures, 
algorithms, abstractions, in a STB Style Header.
Heavily relies on std c runtime, abstracts OS specific.

Idea inspired by @PixelRifts (GitHub) aka @VoxelRifts (Youtube).

### What is inside the toolbox
1. Common (used across other headers)
2. Simple Test Framework (STF)
3. Arena Allocator (arena)

### TODO:
* Better logging for STF
* Better tests (Rewrite test for STF)
* String Views
* Filesystem Interface
* Generic Linked List
* Cross Platform Sockets

### STF:
#### Simple Test Framework
A self contained testing framework
* Building the CLI:
```c
// tests/stf.c
#define STF_IMPL_CLI
#include "stf.h"
```
```sh
gcc ./tests/stf.c -o stf
```
* Building a test:
```c
// TODO: Make better example of STF test module
// tests/stf_module.c
#define STF_IMPL_MOD
#include "stf.h"

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
```
```sh
gcc ./tests/stf_module.c -shared -fPIC -o stf_module.dll
```
* Using the CLI:
1. List all tests avilable in module
```sh
stf ./stf_module.dll list
```
```txt  
[TEST]: "not_implemented_test" -- A test with no implementation --
[TEST]: "test" -- test --
```
2. A non implemented test:
```sh
stf ./stf_module.dll not_implemented_test
```
```txt
[CMD]: stf ./stf_module.dll not_implemented_test
[ERROR]: Unable to load target "not_implemented_test" from a module
[ERROR]: module "./stf_module.dll" registers "not_implemented_test" test, but doesn't implement it
[TEST]: Module: "./stf_module.dll" Test: "not_implemented_test" Result: 1
```
3. An implemented test:
```sh
stf ./stf_module.dll test
```
```txt
[CMD]: stf ./stf_module.dll test
0000000000000000
[TEST]: Module: "./stf_module.dll" Test: "test" Result: 0
```
4. Running all tests in a module:
```sh
stf ./stf_module.dll all
```
```txt
[CMD]: stf ./stf_module.dll not_implemented_test
[ERROR]: Unable to load target "not_implemented_test" from a module
[ERROR]: module "./stf_module.dll" registers "not_implemented_test" test, but doesn't implement it
[TEST]: Module: "./stf_module.dll" Test: "not_implemented_test" Result: 1
[CMD]: stf ./stf_module.dll test
0000000000000000
[TEST]: Module: "./stf_module.dll" Test: "test" Result: 0
[TEST] 2 tests executed
```
#### STF is mostly implemented, theres only the need for better logging and better test modules examples, you can check out stf.h for more info