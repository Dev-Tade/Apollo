#ifndef APOLLO_COMMON_H
#define APOLLO_COMMON_H

#include <stdint.h>

/* 
  -- Helper macro to declare header functions --

  On definition:
  #include "common.h"

  would define APOLLO_DEF as extern

  On an implementation:
  #define APOLLO_IMPL
  #include "common.h"

  would define APOLLO_DEF as static

  definition and implementation in same header:
  #ifndef MY_HEADER_GUARD
  #define MY_HEADER_GUARD

  APOLLO_DEF void my_function(...);
  
  #endif // !MY_HEADER_GUARD
  
  #ifdef MY_HEADER_IMPLEMENTATION
  
  APOLLO_DEF my_function(...) {...}
  
  #endif // !MY_HEADER_IMPLEMENTATION
*/
#ifdef APOLLO_IMPL
  #undef APOLLO_DEF
  #define APOLLO_DEF static
#else
  #undef APOLLO_DEF
  #define APOLLO_DEF extern
#endif

/*
  -- Helper macro to determine underlying OS --
  When system is Windows defines APOLLO_SYS_WINDOWS
  if APOLLO_SYS_UNIX hasn't been defined and vice versa.
  Will trigger an error if OS isn't one of them

  Use case:
  void *my_mmap(...)
  {
    #if defined(APOLLO_SYS_WINDOWS)
      -- use CreateFileMapping, MapViewOfFile impl
    #elif defined(APOLLO_SYS_UNIX)
      -- use mmap impl
    #endif
  }
*/
#if (defined(_WIN32) || defined(_WIN64))
  #ifndef APOLLO_SYS_UNIX
  #define APOLLO_SYS_WINDOWS
  #endif
#elif (defined(__unix__) || defined(__unix) || defined(__linux__) || (defined(__APPLE__) && defined(__MACH__)))
  #ifndef APOLLO_SYS_WINDOWS
  #define APOLLO_SYS_UNIX
  #endif
#else
  #error "Underlying OS is not compilant to neither Windows or Unix"
#endif

#endif // !APOLLO_COMMON_H