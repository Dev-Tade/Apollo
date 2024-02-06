#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define STRVIEW_IMPLEMENTATION
#include "../strview.h"

int main() {
  StrView hello = strview_fromParts("Hello World", 5);
  StrView base = strview_fromCStr("AaAa BbBb");

  StrView ba = strview_chopByDelim(base, ' ');
  StrView se = strview_nextDelim(base, ' ');
  se = strview_next(se);

  char *deallocate = strview_toCStr(hello);
  printf("%s\n", deallocate);
  free(deallocate);

  char buff[128] = {0};
  if (strview_intoBuff(base, buff, 128)) {
    printf("%s\n", buff);
  }

  if (strview_intoBuff(ba, buff, 128)) {
    printf("%s\n", buff);
  }

  if (strview_intoBuff(se, buff, 128)) {
    printf("%s\n", buff);
  }

  StrView xyz = strview_fromCStr("1234");
  fputc(strview_getc(xyz), stdout);
  printf("%llu\n", strview_toU64(xyz));

  return 0;
}