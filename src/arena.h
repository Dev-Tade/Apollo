#ifndef ARENA_H
#define ARENA_H

#include "apollo.h"

/*
  -- Arena --
  Simple linear memory Arena, with no
  support for individual deallocations.
  Single release operation frees entire Arena.
*/

/* 
  -- Arena Struct/Type --
  @warning Do not modify directly, only do so
  if you comprehend its innerworkings,
  do it at your own risk because it may break
  your application if you do so incorrectly.  
*/
typedef struct _Arena
{
  void *base;
  size_t end;
  size_t ptr;
} Arena;

#define ARENA() ((Arena){0})

/*
  -- Initialize memory for the Arena --
  @param arena: Pointer to an Arena.
  @param size: Amount of bytes for the arena.
  @return `false`: On init failed
  (NULL arena or failed to allocate).
*/
APOLLO_DEF bool arena_init(Arena *arena, size_t size);

/*
  -- Deallocate memory of the Arena --
  @param arena: Pointer to an Arena.
*/
APOLLO_DEF void arena_free(Arena *arena);

/*
  -- Get memory from the Arena --
  @param arena: Pointer to an Arena
  @param count: Amount of bytes to reserve
  @return `pointer` to block of reserved memory,
  NULL if no more space is avilable in the Arena
  or NULL arena
*/
APOLLO_DEF void *arena_reserve(Arena *arena, size_t count);

/*
  -- Get memory left in the Arena --
  @param arena: Pointer to an Arena
  @return `Number` of bytes left in arena,
  zero if Arena is NULL
*/
APOLLO_DEF size_t arena_left(Arena *arena);

/*
  -- Get memory used in the Arena --
  @param arena: Pointer to an Arena
  @return `Number` of bytes used in arena,
  zero if Arena is NULL
*/
APOLLO_DEF size_t arena_used(Arena *arena);

#endif //!ARENA_H

#ifdef ARENA_IMPL

#define APOLLO_IMPL
#include "apollo.h"

APOLLO_DEF bool arena_init(Arena *arena, size_t size)
{
  if (arena == NULL) return false;

  arena->base = APOLLO_ALLOC(size);
  if (arena->base == NULL) return false;

  APOLLO_MEMSET(arena->base, 0, arena->end);
  arena->end = size;
  arena->ptr = 0;

  return true;
}

APOLLO_DEF void arena_free(Arena *arena)
{
  APOLLO_FREE(arena->base);
}

APOLLO_DEF void *arena_reserve(Arena *arena, size_t count)
{
  if (arena == NULL) return NULL;
  if ((arena->ptr + count) >= arena->end) return NULL;

  void *ret = (void *)((uintptr_t)arena->base + arena->ptr);
  arena->ptr += count;

  return ret;
}

APOLLO_DEF size_t arena_left(Arena *arena)
{
  if (arena == NULL) return 0;
  
  return (arena->end - arena->ptr);
}

APOLLO_DEF size_t arena_used(Arena *arena)
{
  if (arena == NULL) return 0;

  return (arena->ptr);
}

#endif //!ARENA_IMPL