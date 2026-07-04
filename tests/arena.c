#define STF_IMPL_MOD
#include "../src/stf.h"

#define ARENA_IMPL
#include "../src/arena.h"

#include <string.h>
#include <stdio.h>

static Arena test_arena;

bool arena_init_test(void)
{
  return arena_init(&test_arena, 128);
}

bool arena_alloc_test(void)
{ 
  size_t arena_size = sizeof(uint64_t) + 64;
  printf("initializing an arena of %zu bytes\n", arena_size);
  arena_init(&test_arena, arena_size);

  // Allocate a u64 
  uint64_t *my_u64 = (uint64_t*)arena_alloc(&test_arena, sizeof(uint64_t));
  if (!my_u64) {
    fprintf(stderr, "Couldn't allocate u64\n");
    return false;
  }

  *my_u64 = 0xc0ffee12900daf00;
  printf("my_u64: %p -> %lu\n", my_u64, *my_u64);

  // Allocate a string of 64 characters
  size_t my_string_size = 64;
  char *my_string = (char *)arena_alloc(&test_arena, my_string_size);
  if (!my_string) {
    fprintf(stderr, "Couldn't allocate my_string of size %zu\n", my_string_size);
    return false;
  }

  char *my_string_literal = "coffee is good_af";
  APOLLO_MEMCPY(my_string, my_string_literal, 18);
  printf("my_string: %p -> %s\n", my_string, my_string);

  return true;
}

bool arena_remaining_test(void)
{
  arena_init(&test_arena, 16);

  size_t remaining_bytes = arena_left(&test_arena);
  printf("there are %zu bytes are left on the arena\n", remaining_bytes);

  printf("allocating some memory\n");
  arena_alloc(&test_arena, sizeof(uint32_t) * 3);

  remaining_bytes = arena_left(&test_arena);
  printf("there are %zu bytes are left on the arena\n", remaining_bytes);


  return remaining_bytes > 0;
}

bool arena_used_test(void)
{
  arena_init(&test_arena, 16);

  size_t used_bytes = arena_used(&test_arena);
  printf("there are %zu used bytes on the arena\n", used_bytes);

  printf("allocating some memory\n");
  arena_alloc(&test_arena, sizeof(uint32_t) * 3);

  used_bytes = arena_used(&test_arena);
  printf("there are %zu used bytes on the arena\n", used_bytes);

  return used_bytes > 0;
}

bool arena_alloc_no_space_left(void)
{
  arena_init(&test_arena, 16);
  return !arena_alloc(&test_arena, arena_left(&test_arena) + 20);
}

bool arena_alloc_null_arena(void)
{
  return !arena_alloc(NULL, 20);
}

stf_Test arena_module_tests[] =
{
  {"init", "Initializes an arena", arena_init_test},
  {"alloc", "Allocates memory", arena_alloc_test},
  {"remaining", "Returns remaining space", arena_remaining_test},
  {"used", "Returns used space", arena_used_test},
  {"noleft", "Allocation fails when full", arena_alloc_no_space_left},
  {"null", "Allocation with NULL arena", arena_alloc_null_arena},
};

STF_MODULE_EXPORTS(arena_module_tests);