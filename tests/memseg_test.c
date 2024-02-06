#define MEMSEG_IMPLEMENTATION
#include "../memseg.h"


int main() {
  MemSeg main;
  memseg_init(&main, 64);
  printf("base:%p\nloc:%d\nmax:%d\n", main.base, main.loc, main.max);
  
  char* base = "Hello World";

  void* str = memseg_alloc(&main, 32);
  printf("base = %d\nstr = %d\n# = %d\n", main.base, str, (uint64_t)str-(uint64_t)base);
  memcpy(str, base, 12);

  void * ints = memseg_alloc(&main, sizeof(int)*7);
  for (int i=0; i<8; i++) {
    memcpy((void*)((uint64_t)ints + i*sizeof(int)), &i, 4);
  }

  uint8_t meta = 0;
  void * err = memseg_alloc(&main, 16);

  if (1) {
    printf("str: %s\n", (char*)str);
    printf("ints: ");
    for (int i=0; i<8; i++) {
      printf("%d ", *((int*)((uint64_t)ints+i*sizeof(int))));
    }
    printf("\n");
  }
  
  memseg_free(&main);
}