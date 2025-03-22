#define STF_IMPL_MOD
#include "../src/stf.h"

#define ARENA_IMPL
#include "../src/arena.h"

#include <string.h>
#include <stdio.h>

void stf_module_list(STF_MODULE_LIST)
{
  stf_test_add(MODULE_LIST, TEST(arena_init_test, "inits an arena"));
  stf_test_add(MODULE_LIST, TEST(arena_reserve_test, "inits an arena, allocates a u64 and string of 64 chars"));
  stf_test_add(MODULE_LIST, TEST(arena_remaining_test, "inits an arena, reports it remaining space"));
  stf_test_add(MODULE_LIST, TEST(arena_used_test, "inits an arena, reports it used space"));
  stf_test_add(MODULE_LIST, TEST(arena_reserve_no_space_left, "tries to allocate but runs out of space"));
  stf_test_add(MODULE_LIST, TEST(arena_reserve_null_arena, "tries to allocate on an invalid arena"));
}

static Arena test_arena;

bool arena_init_test(void)
{
  return arena_init(&test_arena, 128);
}

bool arena_reserve_test(void)
{ 
  size_t arena_size = sizeof(uint64_t) + 64;
  printf("initializing an arena of %zu bytes\n", arena_size);
  arena_init(&test_arena, arena_size);

  // Allocate a u64 
  uint64_t *my_u64 = (uint64_t*)arena_reserve(&test_arena, sizeof(uint64_t));
  if (!my_u64) {
    fprintf(stderr, "Couldn't allocate u64\n");
    return false;
  }

  *my_u64 = 0xc0ffee12900daf00;
  printf("my_u64: %p -> %llu\n", my_u64, *my_u64);

  // Allocate a string of 64 characters
  size_t my_string_size = 64;
  char *my_string = (char *)arena_reserve(&test_arena, my_string_size);
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
  arena_reserve(&test_arena, sizeof(uint32_t) * 3);

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
  arena_reserve(&test_arena, sizeof(uint32_t) * 3);

  used_bytes = arena_used(&test_arena);
  printf("there are %zu used bytes on the arena\n", used_bytes);

  return used_bytes > 0;
}

bool arena_reserve_no_space_left(void)
{
  arena_init(&test_arena, 16);
  return !arena_reserve(&test_arena, arena_left(&test_arena) + 20);
}

bool arena_reserve_null_arena(void)
{
  return !arena_reserve(NULL, 20);
}