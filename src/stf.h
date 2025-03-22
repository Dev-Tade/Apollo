#ifndef STF_H
#define STF_H

#include "apollo.h"
#include <stdio.h>

/*
  -- Simple Test Framework --
   # Terms:
   > "module" = dynamic library / shared object (.dll, .so, .dylib)
   > "list" and "all" are test cases implemented by stf CLI
   # Command Line:
   > stf <target-file.module> list   -- List all tests inside target module
   > stf <target-file.module> all    -- Run all tests inside target module
   > stf <target-file.module> <test> -- Run <test> from target module
   # Writing Test:
   > Any test module should include a function called stf_module_list with
   the following signature `void stf_module_list(STF_MODULE_LIST)`
   > The purpose of `stf_module_list` is register all the test in the module via:
   stf_test_add(Test_List test_list, Test *test), its arguments are
   pretty simple, `test_list` you can get this by using: `MODULE_LIST` macro
  `test` argument is a pointer to a Test struct
   > Building a Test structure: you can do it by using `TEST` macro with following
   arguments: `TEST(function_name, description, argument)`, `function_name` is the
   identifier of C function to run (max length 24 characters), `description` is 32
   characters and argument is a pointer to what you want to pass to the test when
   its ran. It could be a static, stack or heap allocated object, if you don' t want
   any just use NULL or 0
   > What does a `test` return: true if succeeded or false if it didn't,
  for example something based on a condition:
   > A very minimal example:
   `
     bool test_n1(void *argument)
     {
       if ( (*(uint32_t *)argument)  == 1) return true;
       return false;
     }
     bool test_n1(uint32_t argument)
     {
       // Cast is not needed since uint32 fits on a pointer on 32/64 bits 
       if (argument == 0) return true;
       return false;
     }

     void stf_module_list(STF_MODULE_LIST)
     {
       stf_test_add(MODULE_LIST, TEST(test_n1, "Should return 1 when 1 is feed", 1));
       stf_test_add(test_list, TEST(test_n2, "Should return 1 when 0 is feed", 0));
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
#define __FN_STR__(x) #x

/*
  -- Helper macro to build a Test Case --
*/
#define TEST(function, description, argument) (\
  &(Test){__FN_STR__(function), description, argument}\
)

/*
  -- Test_List Type/Struct --
  Dynamic array containing entries for every test
*/
typedef struct _stf_test_list
{
  Test *tests;
  size_t count;
  size_t capacity;
} Test_List;

/*
  -- Helper macro to initialize an empty Test_List --
*/
#define TEST_LIST() ((Test_List){0})

/*
  -- Helper macro for stf_module_list argument --
*/

#define STF_MODULE_LIST Test_List *__module_list__

/*
  -- Helper macro for stf_test_add "list" parameter
*/

#define MODULE_LIST __module_list__

/*
  -- Register a test entry into a Test_List --
  Should be called on stf_test_list for adding
  test entries, see "Writing Tests"
*/
APOLLO_DEF void stf_test_add(Test_List *list, Test *test);

#endif //!STF_H

#ifdef STF_IMPL_CLI

#define APOLLO_IMPL
#include "apollo.h"

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
  @param quiet: 'true' if no error messages should be logged
  @return `Target` function pointer, `NULL` if couldn't find `target`
  @note Used internally by stf_cli
*/
APOLLO_DEF Target stf_module_target(Module module, const char *target, bool quiet);

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
  -- List tests of a module --
  @param stf_exec: Path to stf CLI binary (C string)
  @param stf_target: Path to module to load (C string)
  @return `true` on success, `false` on error (prints errors)
  @note Used internally by stf_cli
*/
APOLLO_DEF bool stf_cli_list(const char *stf_exec, const char *stf_target);

/*
  -- Run all tests of a module --
  @param stf_exec: Path to stf CLI binary (C string)
  @param stf_target: Path to module to load (C string)
  @return `true` on success, `false` on error (prints errors)
  @note Used internally by stf_cli
*/
APOLLO_DEF bool stf_cli_all(const char *stf_exec, const char *stf_target);

/*
  -- Run <stf_test> of a module --
  @param stf_exec: Path to stf CLI binary (C string)
  @param stf_target: Path to module to load (C string)
  @param stf_test: Name of test to run (C string)
  @return `true` on success, `false` on error (prints errors)
  @note Used internally by stf_cli
*/
APOLLO_DEF bool stf_cli_test(
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
    if (!stf_cli_list(stf_exec, stf_target)) return 1;
  } else if (strcmp(stf_action, "all") == 0) {
    if (!stf_cli_all(stf_exec, stf_target)) return 1;
  } else {
    return !stf_cli_test(stf_exec, stf_target, stf_action);
  }

  return 0;
}

APOLLO_DEF void stf_cli_manual(void)
{
  stf_log("-- Simple Test Framework --\n", 0);
  stf_log(" # Terms:\n", 0);
  stf_log(" > \"module\" = dynamic library / shared object (.dll, .so, .dylib)\n", 0);
  stf_log(" > \"list\" and \"all\" are test cases implemented by stf CLI\n", 0);
  stf_log(" # Command Line:\n", 0);
  stf_log(" > stf <target-file.module> list   -- List all tests inside target module\n", 0);
  stf_log(" > stf <target-file.module> all    -- Run all tests inside target module\n", 0);
  stf_log(" > stf <target-file.module> <test> -- Run <test> from target module\n", 0);
  stf_log(" # Writing Tests: Checkout stf.h\n", 0);
}

APOLLO_DEF Module stf_module_load(const char *module_path)
{
  Module module = NULL;
#if defined(APOLLO_SYS_WINDOWS)
  module = (void *)LoadLibraryA(module_path);
#elif defined(APOLLO_SYS_UNIX)
  module = dlopen(module_path, RTLD_LAZY);
#endif

  if (!module) {
    stf_err("[ERROR]: An error ocurred while loading module \"%s\"\n", module_path);
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

APOLLO_DEF Target stf_module_target(Module module, const char *target, bool quiet)
{
  Target _target = NULL;
#if defined(APOLLO_SYS_WINDOWS)
  _target = (Target)GetProcAddress(module, target);
#elif defined(APOLLO_SYS_UNIX)
  _target = (Target)dlsym(module, target);
#endif

  if (!_target && !quiet) {
    stf_err("[ERROR]: Unable to load target \"%s\" from a module\n", target);
  }

  return _target;
}

APOLLO_DEF bool stf_module_load_test_list(const char *stf_target, Module module, Test_List *test_list)
{
  // Checking for NULL module should be done by caller
  Target stf_module_list_entry = stf_module_target(module, "stf_module_list", true);
  
  if (!stf_module_list_entry) {
    stf_err(
      "[ERROR]: Entry point: \"stf_module_list\" missing from \"%s\"\n",
      stf_target
    );

    stf_module_unload(module);
    return false;
  }

  size_t prev_count = test_list->count;
  stf_module_list_entry(test_list);

  if (test_list->count <= prev_count) {
    stf_err(
      "[ERROR]: module \"%s\" \"stf_module_list\" didn't add any test\n",
      stf_target
    );

    stf_module_unload(module);
    return false;
  }

  return true;
}

APOLLO_DEF bool stf_cli_list(const char *stf_exec, const char *stf_target)
{
  Module module = stf_module_load(stf_target);
  if (!module) return false;

  Test_List test_list = TEST_LIST();
  
  if (!stf_module_load_test_list(stf_target, module, &test_list)) return false;

  for (size_t i=0; i < test_list.count; ++i) {
    stf_log("[TEST]: \"%s\" -- %s --\n",
      test_list.tests[i].target_string,
      test_list.tests[i].target_description
    );
  }

  stf_module_unload(module);
  return true;
}

APOLLO_DEF bool stf_cli_all(const char *stf_exec, const char *stf_target)
{
  Module module = stf_module_load(stf_target);
  if (!module) return false;

  Test_List test_list = TEST_LIST();

  if (!stf_module_load_test_list(stf_target, module, &test_list)) return false;

  size_t exec_count = test_list.count;
  size_t succeeded_count = 0;

  for (uint32_t i=0; i<exec_count; ++i) {
    Test *test = &test_list.tests[i];

    char buff[255] = {0};
    snprintf(buff, 255, "%s %s %s\n", stf_exec, stf_target, test->target_string);
    succeeded_count += (!system(buff));
    stf_log("\n", 0);
  }

  stf_log(
    "[INFO]: Executed %zu tests, only %zu succeeded (%d%%)\n",
    exec_count,
    succeeded_count,
    succeeded_count * 100 / exec_count
  );
  stf_module_unload(module);
  return true;
}

APOLLO_DEF bool stf_cli_test(
  const char *stf_exec,
  const char *stf_target,
  const char *stf_test
)
{
  Module module = stf_module_load(stf_target);
  if (!module) return false;

  Test_List test_list = TEST_LIST();

  if (!stf_module_load_test_list(stf_target, module, &test_list)) return false;

  for (uint32_t i=0; i<test_list.count; ++i) {
    Test *test = &test_list.tests[i];
    
    if (strcmp(stf_test, test->target_string) == 0) {
      Target target = stf_module_target(module, test->target_string, true);
      
      if (target == NULL) {
        stf_err(
          "[INFO]: Ignoring case: \"%s\" (registered in \"%s\", unable to resolve its address)\n",
          stf_test,
          stf_target
        );
        return false;
      }

      stf_log(
        "[INFO]: Executing case: \"%s\" (from \"%s\") -- %s --\n",
        test->target_string, stf_target, test->target_description
      );

      bool succeeded = target(test->argument);

      stf_log(
        "[INFO]: Finished case: \"%s\" (from \"%s\") -- %s (%s) --\n",
        test->target_string, stf_target,  
        succeeded ? "succeeded" : "didn't succeeded",
        succeeded ? "true" : "false"
      );

      return succeeded;
    }
  }
  
  stf_err("[ERROR]: No test named \"%s\" on module %s\n", stf_test, stf_target);
  stf_module_unload(module);
  return false;
}

#endif //!STF_IMPL_CLI

#ifdef STF_IMPL_MOD

#define APOLLO_IMPL
#include "apollo.h"

APOLLO_DEF void stf_test_add(Test_List *list, Test *test)
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