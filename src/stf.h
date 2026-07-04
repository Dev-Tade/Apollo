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
  const char *name;
  const char *description;
  stf_Target target_function;
} stf_Test;

/*
  -- ModuleExport Type/Struct --
  Sized array for decaying pointer containing module 
  stf_Test[], used when exporting tests from a module.
*/
typedef struct _stf_module_export
{
  size_t count;
  stf_Test *items;
} stf_ModuleExport;

#endif //!STF_H

#ifdef STF_IMPL_CLI

#define APOLLO_IMPL
#include "apollo.h"

/*
  -- Module Type --
  Type alias to OS dynamic library / shared object handle
*/
typedef void *stf_Module;

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
  @return `void *` pointer, `NULL` if couldn't find `target`
  @note Used internally by stf_cli
*/
APOLLO_DEF void * stf_module_load_symbol(stf_Module module, const char *target, bool quiet);

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
  @param stf_target: Path to module to load (C string)
  @return `true` on success, `false` on error (prints errors)
  @note Used internally by stf_cli
*/
APOLLO_DEF bool stf_cli_list(const char *stf_target);

/*
  -- Run all tests of a module --
  @param stf_target: Path to module to load (C string)
  @return `true` on success, `false` on error (prints errors)
  @note Used internally by stf_cli
*/
APOLLO_DEF bool stf_cli_all(const char *stf_target);

/*
  -- Run <stf_test> of a module --
  @param stf_target: Path to module to load (C string)
  @param stf_test: Name of test to run (C string)
  @return `true` on success, `false` on error (prints errors)
  @note Used internally by stf_cli
*/
APOLLO_DEF bool stf_cli_test(
  const char *stf_target,
  const char *stf_test
);

/*
  -- Run <test> from <stf_target> module --
  @param stf_target: Path to module to load (C string)
  @param test: The test that is going to be ran
  @return `false` on error (prints errors) or the result 
  of the test ran 
  @note Used internally by stf_cli
*/
APOLLO_DEF bool stf_run_test(
  const char *stf_target,
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

  const char *stf_target = argv[1];
  const char *stf_action = argv[2];
  
  if (strcmp(stf_action, "list") == 0) {
    if (!stf_cli_list(stf_target)) return 1;
  } else if (strcmp(stf_action, "all") == 0) {
    if (!stf_cli_all(stf_target)) return 1;
  } else {
    return !stf_cli_test(stf_target, stf_action);
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

APOLLO_DEF void *stf_module_load_symbol(stf_Module module, const char *target, bool quiet)
{
  void *_symbol = NULL;
#if defined(APOLLO_SYS_WINDOWS)
  _symbol = GetProcAddress(module, target);
#elif defined(APOLLO_SYS_UNIX)
  _symbol = dlsym(module, target);
#endif

  if (!_symbol && !quiet) {
    stf_err("[ERROR]: Unable to load target \"%s\" from a module\n", target);
  }

  return _symbol;
}

APOLLO_DEF bool stf_module_load_tests(const char *stf_target, stf_Module module, stf_ModuleExport *tests)
{
  // Checking for NULL module should be done by caller
  stf_ModuleExport *stf_module = (stf_ModuleExport *)stf_module_load_symbol(module, "stf_module_exports", true);
  
  if (!stf_module) {
    stf_err(
      "[ERROR]: Missing export: \"stf_module_exports\" from \"%s\"\n",
      stf_target
    );

    stf_module_unload(module);
    return false;
  }

  if (stf_module->items == NULL) {
    stf_err(
      "[ERROR]: Invalid pointer: \"stf_module_exports->items\" (NULL) from \"%s\"\n",
      stf_target
    );

    stf_module_unload(module);
    return false; 
  }

  if (stf_module->count == 0) {
    stf_err(
      "[ERROR]: Invalid count: \"stf_module_exports->count\" (%zu) from \"%s\"\n",
      stf_module->count, stf_target
    );

    stf_module_unload(module);
    return false; 
  }
  
  tests->count = stf_module->count;
  tests->items = stf_module->items;

  return true;
}

APOLLO_DEF bool stf_cli_list(const char *stf_target)
{
  stf_Module module = stf_module_load(stf_target);
  if (!module) return false;

  stf_ModuleExport tests = {0};
  if (!stf_module_load_tests(stf_target, module, &tests)) return false;

  for (size_t i=0; i < tests.count; ++i) {
    stf_log("[TEST]: \"%s\" -- %s --\n",
      tests.items[i].name,
      tests.items[i].description
    );
  }

  stf_module_unload(module);
  return true;
}

APOLLO_DEF bool stf_cli_all(const char *stf_target)
{
  stf_Module module = stf_module_load(stf_target);
  if (!module) return false;

  stf_ModuleExport tests = {0};
  if (!stf_module_load_tests(stf_target, module, &tests)) return false;

  size_t exec_count = tests.count;
  size_t succeeded_count = 0;

  for (uint32_t i=0; i<exec_count; ++i) {
    stf_Test *test = &tests.items[i];
    succeeded_count += stf_run_test(stf_target, test);
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
  const char *stf_target,
  const char *stf_test)
{
  stf_Module module = stf_module_load(stf_target);
  if (!module) return false;

  stf_ModuleExport tests = {0};
  if (!stf_module_load_tests(stf_target, module, &tests)) return false;

  for (uint32_t i=0; i<tests.count; ++i) {
    stf_Test *test = &tests.items[i];
    
    if (strcmp(stf_test, test->name) == 0) 
      return stf_run_test(stf_target, test);
  }
  
  stf_err("[ERROR]: No test named \"%s\" on module %s\n", stf_test, stf_target);
  stf_module_unload(module);
  return false;
}

APOLLO_DEF bool stf_run_test(
  const char *stf_target,
  stf_Test *test)
{
  stf_Target target = test->target_function;

  if (target == NULL) {
    stf_err(
      "[INFO]: Ignoring test case: \"%s\" (registered in \"%s\") unable to resolve it's address\n",
      test->name,
      stf_target
    );

    return false;
  }

  stf_log(
    "[INFO]: Executing test case: \"%s\" (from \"%s\") -- %s --\n",
    test->name, stf_target, test->description
  );

  bool succeeded = target();

  stf_log(
    "[INFO]: Finished test case: \"%s\" (from \"%s\") -- %s (%s) --\n",
    test->name, stf_target,  
    succeeded ? "succeeded" : "didn't succeeded",
    succeeded ? "true" : "false"
  );

  return succeeded;
}

#endif //!STF_IMPL_CLI

#ifdef STF_IMPL_MOD

#define APOLLO_IMPL
#include "apollo.h"

#define STF_MODULE_EXPORTS(tests)         \
  stf_ModuleExport stf_module_exports =   \
  {                                       \
    sizeof((tests)) / sizeof(stf_Test),   \
    (tests),                              \
  }                                       \

#endif //!STF_IMPL_MOD