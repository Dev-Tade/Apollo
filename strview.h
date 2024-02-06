#ifndef STRVIEW_H
#define STRVIEW_H

#ifndef APOLLO_DEF
#define APOLLO_DEF
#else
#undef APOLLO_DEF
#define APOLLO_DEF
#endif

#include <stdint.h>

typedef struct {
  const char *data;
  size_t size;
} StrView;

APOLLO_DEF StrView strview_fromCStr(char *cstr);
APOLLO_DEF StrView strview_fromParts(char *cstr, size_t amount);
APOLLO_DEF StrView strview_chopByDelim(StrView src, char delim);
APOLLO_DEF StrView strview_nextDelim(StrView src, char delim);
APOLLO_DEF StrView strview_next(StrView sv);

APOLLO_DEF char strview_getc(StrView sv);

APOLLO_DEF char *strview_toCStr(StrView sv);
APOLLO_DEF uint64_t strview_toU64(StrView sv);
APOLLO_DEF bool strview_intoBuff(StrView sv, char *buff, size_t buff_s);

#endif

#ifdef STRVIEW_IMPLEMENTATION

#ifndef APOLLO_DEF
#define APOLLO_DEF static
#else
#undef APOLLO_DEF
#define APOLLO_DEF static
#endif

#include <string.h>
#include <stdlib.h>

APOLLO_DEF StrView strview_fromCStr(char *cstr) {
  StrView x = {.data=cstr, .size=strlen(cstr)};
  return x;
}

APOLLO_DEF StrView strview_fromParts(char *cstr, size_t amount) {
  StrView x = {.data=cstr, .size=amount};
  return x;
}

APOLLO_DEF StrView strview_chopByDelim(StrView src, char delim) {
  size_t c = 0;
  while (c < src.size && src.data[c] != delim) c++;

  StrView x = {.data=src.data, .size=c};
  return x;
}

APOLLO_DEF StrView strview_nextDelim(StrView src, char delim) {
  char *next = strchr((char*)src.data, delim);
  if (next == NULL) return src;
  StrView x = {.data=next, .size=src.size - (next-src.data)};
  return x;
}

APOLLO_DEF StrView strview_next(StrView sv) {
  StrView x = {.data=sv.data+1, .size=sv.size-1};
  return x;
}

APOLLO_DEF char strview_getc(StrView sv) {
  return *sv.data;
}

APOLLO_DEF char *strview_toCStr(StrView sv) {
  char *ret = (char*)malloc(sv.size+1);
  memcpy(ret, sv.data, sv.size);
  ret[sv.size] = 0;

  return ret;
}

APOLLO_DEF uint64_t strview_toU64(StrView sv) {
  char *x = strview_toCStr(sv);
  uint64_t y = atoll(x);
  free(x);
  return y;
}

APOLLO_DEF bool strview_intoBuff(StrView sv, char *buff, size_t buff_s) {
  if (sv.size >= buff_s) return false;
  
  memcpy(buff, sv.data, sv.size);
  buff[sv.size] = 0;
  return true;
}

#endif