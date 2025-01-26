#ifndef APOLLO_COMMON_H
#define APOLLO_COMMON_H

#include <stdint.h>

#ifdef APOLLO_IMPL
  #undef APOLLO_DEF
  #define APOLLO_DEF static
#else
  #undef APOLLO_DEF
  #define APOLLO_DEF extern
#endif

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