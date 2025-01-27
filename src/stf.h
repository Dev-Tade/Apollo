#ifndef STF_H
#define STF_H

#include "common.h"
#include <stdio.h>

/*
  -- Simple Test Framework --
   # Terms:
   > "module" = dynamic library / shared object (.dll, .so, .dylib)
   > "list" and "all" = <test> targets implemented by stf CLI
   # Command Line:
   > stf <target-file.module> list   -- List all tests inside target module
   > stf <target-file.module> all    -- Run all tests inside target module
   > stf <target-file.module> <test> -- Run <test> from target module
   # Writing Test:
   > Any test module should include a function called stf_module_list with
   the following signature `uint32_t stf_module_list(void *argument)`
   > The purpose of `stf_module_list` is register all the test in the module via:
   stf_module_list_add(ModuleList module_list, Test *test), its arguments are
   pretty simple, `module_list` you can get this by doing:
   `ModuleList module_list = MODULE_LIST_ARGUMENT(argument);` at the beggining
   of `stf_module_list`. `test` argument is a pointer to a Test struct
   > Building a Test structure: you can do it by using `TEST` macro with following
   arguments: `TEST(function_name, description, argument)`, `function_name` is the
   identifier of C function to run (max length 24 characters), `description` is 32
   characters and argument is a pointer to what you want to pass to the test when
   is ran. It could be a static, stack or heap allocated object, if you don' t want
   > What does `stf_module_list` return: STF_SUCCESS, if not the CLI will print a
   detailed error
   > What does a `test` return: whatever value you want, for example something based
   on a condition:
   > A very minimal example:
   `
     static int myValue = 0;
     uint32_t my_positive_test(void *argument)
     {
       if ((*(uint32_t *)argument) == 1) return STF_SUCCESS;
       return STF_FAIL;
     }
     uint32_t my_negative_test(void *argument)
     {
       if ((*(uint32_t *)argument) == 0) return STF_SUCCESS;
       return STF_FAIL;
     }

     uint32_t stf_module_list(void *_module_list)
     {
       ModuleList *module_list = MODULE_LIST_ARGUMENT(_module_list);
       myValue = 1;
       stf_module_list_add(module_list, TEST(my_positive_test, "Should return 1 when is feed 1", &myValue));
       myValue = 0;
       stf_module_list_add(module_list, TEST(my_negative_test, "Should return 1 when is feed 0", &myValue));
       return STF_SUCCESS;
     }
    `
*/

/*
  -- Helper macro to printf to stderr --
  @note When no variadic arguments are used, pass zero like so:
  `stf_err(..., 0)`
*/
#define stf_err(fmt, ...) (fprintf(stderr, fmt, __VA_ARGS__))

/*
  -- Helper macro to printf to stdout --
  @note When no variadic arguments are used, pass zero like so:
  `stf_log(..., 0)`
*/
#define stf_log(fmt, ...) (fprintf(stdout, fmt, __VA_ARGS__))

#define STF_FAIL 1
#define STF_SUCCESS 0

/*
  -- Module Type --
  Type alias to OS dynamic library / shared object handle
*/
typedef void *Module;

/*
  -- Target Type --
  Type alias to a function pointer with a test signature.
  Every test written should follow that pattern:
  `bool <test_name>(void *argument)`
*/
typedef uint32_t (*Target)(void *arg);

/*
  -- Test Entry Type/Struct --
  Contains all info about a test
*/
typedef struct _stf_test
{
  char target_string[24];
  char target_description[32];
  void *argument;
} Test;

/*
  -- Helper macro to converting function name to string --
  @note Used internally
*/
#define FUNCTION_NAME_STRING(x) #x

/*
  -- Helper macro to build a Test Entry --
*/
#define TEST(function, description, argument) (\
&(Test){FUNCTION_NAME_STRING(function), description, argument}\
)

/*
  -- ModuleList Type/Struct --
  Dynamic array containing entries for every test
*/
typedef struct _stf_test_list
{
  Test *tests;
  size_t count;
  size_t capacity;
} ModuleList;

/*
  -- Helper macro to initialize an empty ModuleList --
*/
#define MODULE_LIST() ((ModuleList){0})

/*
  -- Helper macro to initialize ModuleList from Target argument --
*/
#define MODULE_LIST_ARGUMENT(x) ((ModuleList*)x)

/*
  -- Register a test entry into a ModuleList --
  Should be called on stf_module_list for adding
  test entries, see "Writing Tests"
*/
APOLLO_DEF void stf_module_list_add(ModuleList *list, Test *test);

#endif //!STF_H

#ifdef STF_IMPL_CLI

#define APOLLO_IMPL
#include "common.h"

/*
  -- Check if file exists --
  @param path: path to the file (C string)
  @return `true` if file exists
  @note Used internally by stf_cli
*/
APOLLO_DEF bool stf_file_exist(const char *path);

/*
  -- Load a module --
  @param module_path: path to module file (C string)
  @return `Module` handle, `NULL` if couldn't load properly
  @note Used internally by stf_cli
*/
APOLLO_DEF Module stf_module_load(const char *module_path);

/*
  -- Load function from module --
  @param module: `Module` to load from
  @param target: Name of the function to load (C string)
  @return `Target` function pointer, `NULL` if couldn't find `target`
  @note Used internally by stf_cli
*/
APOLLO_DEF Target stf_module_target(Module module, const char *target);

/*
  -- Unload a module --
  @param module: `Module` to unload
  @return `May print some errors`
  @note Used internally by stf_cli
*/
APOLLO_DEF void stf_module_unload(Module module);

/*
  -- Print manual for CLI --
  @note Used internally by stf_cli
*/
APOLLO_DEF void stf_cli_manual(void);

/*
  -- List test of a module --
  @param stf_exec: Path to stf CLI binary (C string)
  @param stf_target: Path to module to load (C string)
  @return `STF_SUCCESS`, `STF_ERROR` on error (prints errors)
  @note Used internally by stf_cli
*/
APOLLO_DEF uint32_t stf_cli_list(const char *stf_exec, const char *stf_target);

/*
  -- Run all tests of a module --
  @param stf_exec: Path to stf CLI binary (C string)
  @param stf_target: Path to module to load (C string)
  @return `STF_SUCCESS`, `STF_ERROR` on error (prints errors)
  @note Used internally by stf_cli
*/
APOLLO_DEF uint32_t stf_cli_all(const char *stf_exec, const char *stf_target);

/*
  -- Run <stf_test> of a module --
  @param stf_exec: Path to stf CLI binary (C string)
  @param stf_target: Path to module to load (C string)
  @param stf_test: Name of test to run (C string)
  @return `STF_SUCCESS`, `STF_ERROR` on error (prints errors)
  @note Used internally by stf_cli
*/
APOLLO_DEF uint32_t stf_cli_test(
  const char *stf_exec,
  const char *stf_target,
  const char *stf_test
);

/*
  -- Main entry point of CLI --
  @param argc: libc runtime command line argument count
  @param argv: libc runtime command line arguments array
  @return `exit code`: 1 error or 0 success
  @note Used internally by stf_cli
*/
int main(int argc, char *argv[])
{
  if (argc < 3) {
    stf_cli_manual();
    return 0;
  }

  const char *stf_exec = argv[0];
  const char *stf_target = argv[1];
  const char *stf_action = argv[2];
  
  if (strcmp(stf_action, "list") == 0) {
    if (stf_cli_list(stf_exec, stf_target) != STF_SUCCESS)
      return 1;
  } else if (strcmp(stf_action, "all") == 0) {
    if (stf_cli_all(stf_exec, stf_target) != STF_SUCCESS)
      return 1;
  } else {
    stf_log(
      "[TEST]: Module: \"%s\" Test: \"%s\" Result: %d\n",
      stf_target, stf_action, stf_cli_test(stf_exec, stf_target, stf_action)
    );
  }

  return 0;
}

APOLLO_DEF void stf_cli_manual(void)
{
  stf_log("-- Simple Test Framework --\n", 0);
  stf_log(" # Terms:\n", 0);
  stf_log(" > \"module\" = dynamic library / shared object (.dll, .so, .dylib)\n", 0);
  stf_log(" > \"list\" and \"all\" = <test> targets implemented by stf CLI\n", 0);
  stf_log(" # Command Line:\n", 0);
  stf_log(" > stf <target-file.module> list   -- List all tests inside target module\n", 0);
  stf_log(" > stf <target-file.module> all    -- Run all tests inside target module\n", 0);
  stf_log(" > stf <target-file.module> <test> -- Run <test> from target module\n", 0);
  stf_log(" # Writing Tests: Checkout stf.h\n", 0);
}

APOLLO_DEF bool stf_file_exist(const char *path)
{
  FILE *pFile = fopen(path, "rb");
  if (pFile == NULL) return false;

  fclose(pFile);
  return true;
}

APOLLO_DEF Module stf_module_load(const char *module_path)
{
  Module module = NULL;
#if defined(APOLLO_SYS_WINDOWS)
  module = (void *) LoadLibraryA(module_path);
#elif defined(APOLLO_SYS_UNIX)
  module = dlopen(module_path, RTLD_LAZY);
#endif

  if (module == NULL) {
    stf_err("[ERROR]: An error while loading module \"%s\"\n", module_path);
    exit(1);
  }

  return module;
}

APOLLO_DEF void stf_module_unload(Module module)
{
#if defined(APOLLO_SYS_WINDOWS)
  if(FreeLibrary(module) == 0) {
    stf_err("[ERROR]: An error ocurred while unloading a module\n", NULL);
    exit(1);
  }
#elif defined(APOLLO_SYS_UNIX)
  if (dlclose(module) != 0) {
    stf_err("[ERROR]: An error ocurred while unloading a module\n", NULL);
    exit(1);
  }
#endif
}

APOLLO_DEF Target stf_module_target(Module module, const char *target)
{
  Target _target = NULL;
#if defined(APOLLO_SYS_WINDOWS)
  _target = (Target)GetProcAddress(module, target);
#elif defined(APOLLO_SYS_UNIX)
  _target = (Target)dlsym(module, target);
#endif

  if (_target == NULL) {
    stf_err("[ERROR]: Unable to load target \"%s\" from a module\n", target);
  }

  return _target;
}

APOLLO_DEF uint32_t stf_cli_module_list(const char *stf_target, Module module, ModuleList *module_list)
{
  Target stf_module_list = stf_module_target(module, "stf_module_list");
  if (stf_module_list == NULL) {
    stf_err(
      "[ERROR]: module \"%s\" doesn't provide stf_module_list\n",
      stf_target
    );
    stf_module_unload(module);
    return STF_FAIL;
  }

  uint32_t status = stf_module_list(module_list);
  if (status != STF_SUCCESS) {
    stf_err(
      "[ERROR]: module \"%s\" stf_module_list didn't succeed\n",
      stf_target
    );
    stf_module_unload(module);
    return STF_FAIL;
  }
}

APOLLO_DEF uint32_t stf_cli_list(const char *stf_exec, const char *stf_target)
{
  Module module = stf_module_load(stf_target);
  if (module == NULL) return STF_FAIL;

  ModuleList module_list = MODULE_LIST();
  uint32_t status = stf_cli_module_list(stf_target, module, &module_list);

  if (status != STF_SUCCESS) return status;

  for (size_t i=0; i<module_list.count; ++i) {
    stf_log("[TEST]: \"%s\" -- %s --\n",
      module_list.tests[i].target_string,
      module_list.tests[i].target_description
    );
  }

  stf_module_unload(module);
  return status;
}

APOLLO_DEF uint32_t stf_cli_all(const char *stf_exec, const char *stf_target)
{
  Module module = stf_module_load(stf_target);
  if (module == NULL) return STF_FAIL;

  ModuleList module_list = MODULE_LIST();
  uint32_t status = stf_cli_module_list(stf_target, module, &module_list);
  if (status != STF_SUCCESS) return status;

  for (uint32_t i=0; i<module_list.count; ++i) {
    char buff[255] = {0};
    snprintf(buff, 255, "%s %s %s\n", stf_exec, stf_target, module_list.tests[i].target_string);
    status = system(buff);
  }

  stf_log("[TEST] %d tests executed\n", module_list.count);
  stf_module_unload(module);
  return STF_SUCCESS;
}

APOLLO_DEF uint32_t stf_cli_test(
  const char *stf_exec,
  const char *stf_target,
  const char *stf_test
)
{
  Module module = stf_module_load(stf_target);
  if (module == NULL) return STF_FAIL;

  ModuleList module_list = MODULE_LIST();
  uint32_t status = stf_cli_module_list(stf_target, module, &module_list);
  if (status != STF_SUCCESS) return status;

  stf_log("[CMD]: %s %s %s\n", stf_exec, stf_target, stf_test);

  for (uint32_t i=0; i<module_list.count; ++i) {
    if (strcmp(stf_test, module_list.tests[i].target_string) == 0) {
      Target target = stf_module_target(module, stf_test);
      
      if (target == NULL) {
        stf_err(
          "[ERROR]: module \"%s\" registers \"%s\" test, but doesn't implement it\n",
          stf_target,
          stf_test
        );
        return STF_FAIL;
      }

      return target(0);
    }
  }
  
  stf_err("[ERROR]: No test named \"%s\" on module %s\n", stf_test, stf_target);
  stf_module_unload(module);
  return STF_FAIL;
}

#endif //!STF_IMPL_CLI

#ifdef STF_IMPL_MOD

#define APOLLO_IMPL
#include "common.h"

APOLLO_DEF void stf_module_list_add(ModuleList *list, Test *test)
{
  // Initialize dynamic array if not done yet
  if (list->capacity == 0) {
    // Space for 16 tests by default seems reasonable
    list->capacity = 16;
    list->tests = (Test *)APOLLO_ALLOC(sizeof(Test) * list->capacity);
    if (list->tests == NULL) {
      stf_err("[ERROR]: Unable to allocate space for test list\n", 0);
      return;
    }
  }

  // Reallocate list if capacity exceeded
  if ((list->count + 1) > list->capacity) {
    // Allocate space for another 16 entries
    list->capacity += 16;
    list->tests = (Test *)APOLLO_REALLOC(
      list->tests, 
      sizeof(Test) * list->capacity
    );

    if (list->tests == NULL) {
      stf_err("[ERROR]: Unable to reallocate space for test list\n", 0);
      return;
    }
  }

  // Actually add the test entry
  Test *dest = &list->tests[list->count++];
  if (APOLLO_MEMCPY(dest, test, sizeof(Test)) != dest) {
    stf_err("[ERROR]: Unable setup test entry for: %s\n", test->target_string);
    return;
  }
}

#endif //!STF_IMPL_MOD