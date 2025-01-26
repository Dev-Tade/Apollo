#include "../src/arena.h"

#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  Arena myArena = ARENA();
  
  if (!arena_init(&myArena, 128)) {
    fprintf(stderr, "Failed to initialize memory arena\n");
    return 1;
  }

  uint64_t *myU64 = (uint64_t*)arena_reserve(&myArena, sizeof(uint64_t));
  if (myU64 == NULL) {
    fprintf(stderr, "Arena reserve failed\n");
    return 1;
  }

  *myU64 = 0xc0ffee12900d4e4e;
  printf("myU64: %p -> %llu\n", myU64, *myU64);

  char *myString = (char *)arena_reserve(&myArena, 64);
  if (myString == NULL) {
    fprintf(stderr, "Arena reserve failed\n");
    return 1;
  }

  char *myStringLiteral = "coffee_is_good_hehe";
  APOLLO_MEMCPY(myString, myStringLiteral, 20);
  printf("myString: %p -> %s\n", myString, myString);

  size_t remainingBytes = arena_left(&myArena);
  printf("%d bytes are left on the arena\n", remainingBytes);

  size_t usedBytes = arena_used(&myArena);
  printf("%d bytes are used on the arena\n", usedBytes);

  #if 0
  if (arena_reserve(&myArena, remainingBytes + 20) == NULL) {
    fprintf(stderr, "Arena reserve failed (forced not enough memory)\n");
    return 1;
  }
  #endif
  #if 0
  if (arena_reserve(NULL, remainingBytes + 20) == NULL) {
    fprintf(stderr, "Arena reserve failed (forced not valid Arena)\n");
    return 1;
  }
  #endif

  return 0;
}