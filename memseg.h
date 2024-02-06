#ifndef MEMSEG_H
#define MEMSEG_H

#ifndef APOLLO_DEF
#define APOLLO_DEF
#else
#undef APOLLO_DEF
#define APOLLO_DEF
#endif

#include <stdint.h>

#define KB(x) (x*1024)

typedef struct {
  void *base;
  size_t max;
  size_t loc;
} MemSeg;

/*
  Initialize and allocate memory for a stack managed memseg
  @param memseg: stack address of the memseg
  @param size: size for the memseg
*/
APOLLO_DEF void memseg_init(MemSeg *memseg, size_t size);

/*
  Destroy and deallocate memory for a stack managed memseg
  @param memseg: stack address of the memseg 
*/
APOLLO_DEF void memseg_free(MemSeg *memseg);

/*
  Reserve a memory block in the memseg
  @param memseg: stack address of the memseg
  @param size: amount of memory to allocate
*/
APOLLO_DEF void *memseg_alloc(MemSeg *memseg, size_t size);

#endif

/////////////////////////////////////////
//           IMPLEMENTATION            //
/////////////////////////////////////////

#ifdef MEMSEG_IMPLEMENTATION

#ifndef APOLLO_DEF
#define APOLLO_DEF static
#else
#undef APOLLO_DEF
#define APOLLO_DEF static
#endif

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

APOLLO_DEF void memseg_init(MemSeg *memseg, size_t size) {
  memset(memseg, 0, sizeof(MemSeg));
  memseg->max = size;
  memseg->base = malloc(size);
}

APOLLO_DEF void memseg_free(MemSeg *memseg) {
  free(memseg->base);
}

APOLLO_DEF void *memseg_alloc(MemSeg *memseg, size_t size) {
  if (!(memseg->loc + size > memseg->max)) {
    void* ret = (void*)((uint64_t)memseg->base + memseg->loc);
    memseg->loc += size;

    return ret;
  }

  return NULL;
}

#endif