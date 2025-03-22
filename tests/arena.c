#define ARENA_IMPL
#include "../src/arena.h"

#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  Arena my_arena = ARENA();
  
  if (!arena_init(&my_arena, 128)) {
    fprintf(stderr, "Failed to initialize memory arena\n");
    return 1;
  }

  uint64_t *my_u64 = (uint64_t*)arena_reserve(&my_arena, sizeof(uint64_t));
  if (my_u64 == NULL) {
    fprintf(stderr, "Arena reserve failed\n");
    return 1;
  }

  *my_u64 = 0xc0ffee12900d4e4e;
  printf("my_u64: %p -> %llu\n", my_u64, *my_u64);

  char *my_string = (char *)arena_reserve(&my_arena, 64);
  if (my_string == NULL) {
    fprintf(stderr, "Arena reserve failed\n");
    return 1;
  }

  char *my_string_literal = "coffee_is_good_hehe";
  APOLLO_MEMCPY(my_string, my_string_literal, 20);
  printf("my_string: %p -> %s\n", my_string, my_string);

  size_t remaining_bytes = arena_left(&my_arena);
  printf("%d bytes are left on the arena\n", remaining_bytes);

  size_t used_bytes = arena_used(&my_arena);
  printf("%d bytes are used on the arena\n", used_bytes);

  #if 0
  if (arena_reserve(&my_arena, remaining_bytes + 20) == NULL) {
    fprintf(stderr, "Arena reserve failed (forced not enough memory)\n");
    return 1;
  }
  #endif
  #if 0
  if (arena_reserve(NULL, remaining_bytes + 20) == NULL) {
    fprintf(stderr, "Arena reserve failed (forced not valid Arena)\n");
    return 1;
  }
  #endif

  return 0;
}