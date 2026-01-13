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
  the following signature `void stf_module_list(stf_Tests *)`
  > The purpose of `stf_module_list` is register all the test in the module via:
  stf_register_test(stf_Tests *list, stf_Test *test), its arguments are
  pretty simple, `list` or any other name, it' the argument passed to
  `stf_module_list()`, `test` argument is a pointer to a stf_Test struct
  > Building a Test structure: you can do it by using `TEST` macro with following
  arguments: `STF_TEST_CASE(function_name, description, argument)`, `function_name` is the
  identifier of C function to run (max length 64 characters), `description` is 64
  characters.
  > What does a `test` return?: true if succeeded or false if it didn't.
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
typedef void *stf_Module;

/*
  -- Target Type --
  Type alias to a function pointer with a test signature.
  Every test written should follow that pattern:
  `bool <test_name>(void)`
*/
typedef bool (*stf_Target)(void);

/*
  -- Test Entry Type/Struct --
  Contains all info about a test
*/
typedef struct _stf_test
{
  char target_string[64];
  char target_description[64];
} stf_Test;

/*
  -- Helper macro to converting function name to string --
  @note Used internally
*/
#define __FN_STR__(x) #x

/*
  -- Helper macro to build a Test Case --
*/
#define STF_TEST_CASE(function, description) \
  (&(stf_Test){__FN_STR__(function), description})

/*
  -- Test_List Type/Struct --
  Dynamic array containing entries for every test
*/
typedef struct _stf_test_list
{
  stf_Test *items;
  size_t count;
  size_t capacity;
} stf_Tests;

/*
  -- Register a test entry into a stf_Tests list --
  Should be called on stf_module_list for adding
  test entries, see "Writing Tests"
*/
APOLLO_DEF void stf_register_test(stf_Tests *tests, stf_Test *test);

#endif //!STF_H

#ifdef STF_IMPL_CLI

#define APOLLO_IMPL
#include "apollo.h"

/*
  -- ModuleEntryPoint Type --
  Type alias to a function pointer with a stf_module_list signature.
  Every test file should have that entry point:
  `void stf_module_list(stf_Tests *tests)`
*/
typedef void (*stf_ModuleEntryPoint)(stf_Tests *);

/*
  -- Load a module --
  @param module_path: path to module file (C string)
  @return `stf_Module` handle, `NULL` if couldn't load properly
  @note Used internally by stf_cli
*/
APOLLO_DEF stf_Module stf_module_load(const char *module_path);

/*
  -- Load function from module --
  @param module: `stf_Module` to load from
  @param target: Name of the function to load (C string)
  @param quiet: 'true' if no error messages should be logged
  @return `stf_Target` function pointer, `NULL` if couldn't find `target`
  @note Used internally by stf_cli
*/
APOLLO_DEF stf_Target stf_module_target(stf_Module module, const char *target, bool quiet);

/*
  -- Unload a module --
  @param module: `Module` to unload
  @return `May print some errors`
  @note Used internally by stf_cli
*/
APOLLO_DEF void stf_module_unload(stf_Module module);

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
  -- Run <test> of <module> --
  @param stf_target: Path to module to load (C string)
  @param module: Module from where the test was loaded
  @param test: The test that is going to be ran
  @return `false` on error (prints errors) or the result 
  of the test ran 
  @note Used internally by stf_cli
*/
APOLLO_DEF bool stf_run_test(
  const char *stf_target,
  const stf_Module module,
  stf_Test *test
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

APOLLO_DEF stf_Module stf_module_load(const char *module_path)
{
  stf_Module module = NULL;
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

APOLLO_DEF void stf_module_unload(stf_Module module)
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

APOLLO_DEF stf_Target stf_module_target(stf_Module module, const char *target, bool quiet)
{
  stf_Target _target = NULL;
#if defined(APOLLO_SYS_WINDOWS)
  _target = (stf_Target)GetProcAddress(module, target);
#elif defined(APOLLO_SYS_UNIX)
  _target = (stf_Target)dlsym(module, target);
#endif

  if (!_target && !quiet) {
    stf_err("[ERROR]: Unable to load target \"%s\" from a module\n", target);
  }

  return _target;
}

APOLLO_DEF bool stf_module_load_test_list(const char *stf_target, stf_Module module, stf_Tests *tests)
{
  // Checking for NULL module should be done by caller
  stf_ModuleEntryPoint stf_module_list_entry = (stf_ModuleEntryPoint)stf_module_target(module, "stf_module_list", true);
  
  if (!stf_module_list_entry) {
    stf_err(
      "[ERROR]: Entry point: \"stf_module_list\" missing from \"%s\"\n",
      stf_target
    );

    stf_module_unload(module);
    return false;
  }

  size_t prev_count = tests->count;
  stf_module_list_entry(tests);

  if (tests->count <= prev_count) {
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
  stf_Module module = stf_module_load(stf_target);
  if (!module) return false;

  stf_Tests tests = {0};
  if (!stf_module_load_test_list(stf_target, module, &tests)) return false;

  for (size_t i=0; i < tests.count; ++i) {
    stf_log("[TEST]: \"%s\" -- %s --\n",
      tests.items[i].target_string,
      tests.items[i].target_description
    );
  }

  stf_module_unload(module);
  return true;
}

APOLLO_DEF bool stf_cli_all(const char *stf_exec, const char *stf_target)
{
  stf_Module module = stf_module_load(stf_target);
  if (!module) return false;

  stf_Tests tests = {0};
  if (!stf_module_load_test_list(stf_target, module, &tests)) return false;

  size_t exec_count = tests.count;
  size_t succeeded_count = 0;

  for (uint32_t i=0; i<exec_count; ++i) {
    stf_Test *test = &tests.items[i];
    succeeded_count += stf_run_test(stf_target, module, test);
  }

  stf_log(
    "[INFO]: Executed %zu tests, %zu succeeded (%d%%)\n",
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
  const char *stf_test)
{
  stf_Module module = stf_module_load(stf_target);
  if (!module) return false;

  stf_Tests tests = {0};
  if (!stf_module_load_test_list(stf_target, module, &tests)) return false;

  for (uint32_t i=0; i<tests.count; ++i) {
    stf_Test *test = &tests.items[i];
    
    if (strcmp(stf_test, test->target_string) == 0) 
      return stf_run_test(stf_target, module, test);
  }
  
  stf_err("[ERROR]: No test named \"%s\" on module %s\n", stf_test, stf_target);
  stf_module_unload(module);
  return false;
}

APOLLO_DEF bool stf_run_test(
  const char *stf_target,
  const stf_Module module,
  stf_Test *test)
{
  stf_Target target = stf_module_target(module, test->target_string, true);

  if (target == NULL) {
    stf_err(
      "[INFO]: Ignoring case: \"%s\" (registered in \"%s\") unable to resolve it's address\n",
      test->target_string,
      stf_target
    );

    return false;
  }

  stf_log(
    "[INFO]: Executing case: \"%s\" (from \"%s\") -- %s --\n",
    test->target_string, stf_target, test->target_description
  );

  bool succeeded = target();

  stf_log(
    "[INFO]: Finished case: \"%s\" (from \"%s\") -- %s (%s) --\n",
    test->target_string, stf_target,  
    succeeded ? "succeeded" : "didn't succeeded",
    succeeded ? "true" : "false"
  );

  return succeeded;
}

#endif //!STF_IMPL_CLI

#ifdef STF_IMPL_MOD

#define APOLLO_IMPL
#include "apollo.h"

APOLLO_DEF void stf_register_test(stf_Tests *tests, stf_Test *test)
{
  // Initialize dynamic array if not done yet
  if (tests->capacity == 0) {
    // Space for 16 tests by default seems reasonable
    tests->capacity = 16;
    tests->items = (stf_Test *)APOLLO_ALLOC(sizeof(stf_Test) * tests->capacity);
    if (tests->items == NULL) {
      stf_err("[ERROR]: Unable to allocate space for test list\n", 0);
      return;
    }
  }

  // Reallocate list if capacity exceeded
  if ((tests->count + 1) > tests->capacity) {
    // Allocate space for another 16 entries
    tests->capacity += 16;
    tests->items = (stf_Test *)APOLLO_REALLOC(
      tests->items, 
      sizeof(stf_Test) * tests->capacity
    );

    if (tests->items == NULL) {
      stf_err("[ERROR]: Unable to reallocate space for test list\n", 0);
      return;
    }
  }

  // Actually add the test entry
  stf_Test *dest = &tests->items[tests->count++];
  if (APOLLO_MEMCPY(dest, test, sizeof(stf_Test)) != dest) {
    stf_err("[ERROR]: Unable setup test entry for: %s\n", test->target_string);
    return;
  }
}

#endif //!STF_IMPL_MOD