
/*
  -- ARENA_DEF --
  Storage class specifier
  extern when including header, and none when including implementation.
*/
#ifdef ARENA_IMPL
// Implementation include
#define ARENA_DEF 
#else
// Header include
#define ARENA_DEF extern
#endif //!ARENA_IMPL

#ifndef ARENA_H
#define ARENA_H

#include <stdbool.h>
#include <stddef.h>

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
typedef struct Arena Arena;

struct Arena
{
  void *base;
  size_t end;
  size_t ptr;
};

/*
  -- Initialize memory for the Arena --
  @param arena: Pointer to an Arena.
  @param size: Amount of bytes for the arena.
  @return `false`: On init failed
  (NULL arena or failed to allocate).
*/
ARENA_DEF bool arena_init(Arena *arena, size_t size);

/*
  -- Deallocate memory of the Arena --
  @param arena: Pointer to an Arena.
*/
ARENA_DEF void arena_release(Arena *arena);

/*
  -- Get memory from the Arena --
  @param arena: Pointer to an Arena
  @param count: Amount of bytes to reserve
  @return `pointer` to block of reserved memory,
  NULL if no more space is avilable in the Arena
  or NULL arena
*/
ARENA_DEF void *arena_alloc(Arena *arena, size_t count);

/*
  -- Get memory left in the Arena --
  @param arena: Pointer to an Arena
  @return `Number` of bytes left in arena,
  zero if Arena is NULL
*/
ARENA_DEF size_t arena_left(Arena *arena);

/*
  -- Get memory used in the Arena --
  @param arena: Pointer to an Arena
  @return `Number` of bytes used in arena,
  zero if Arena is NULL
*/
ARENA_DEF size_t arena_used(Arena *arena);

#endif //!ARENA_H

#ifdef ARENA_IMPL

/*
  -- ARENA_MALLOC --
  MAcro that allows overrinding the default allocator function (malloc).
  The signature is ARENA_MALLOC(size)
*/
#ifndef ARENA_MALLOC
#include <stdlib.h>
#define ARENA_MALLOC(size) malloc(size)
#endif //!ARENA_MALLOC

/*
  -- ARENA_FREE --
  MAcro that allows overrinding the default free function (free).
  The signature is ARENA_FREE(block)
*/
#ifndef ARENA_FREE
#include <stdlib.h>
#define ARENA_FREE(block) free(block)
#endif //!ARENA_FREE

/*
  -- ARENA_MEMSET --
  MAcro that allows overrinding the default memset function (memset).
  The signature is ARENA_MEMSET(block, value, size)
*/
#ifndef ARENA_MEMSET
#include <string.h>
#define ARENA_MEMSET(block, value, size) memset(block, value, size)
#endif //!ARENA_MEMSET

ARENA_DEF bool arena_init(Arena *arena, size_t size)
{
  if (!arena) return false;

  arena->base = ARENA_MALLOC(size);
  if (!arena->base) return false;

  arena->end = size;
  arena->ptr = 0;
  ARENA_MEMSET(arena->base, 0, arena->end);

  return true;
}

ARENA_DEF void arena_release(Arena *arena)
{
  ARENA_FREE(arena->base);
}

ARENA_DEF void *arena_alloc(Arena *arena, size_t count)
{
  if (!arena) return NULL;
  if ((arena->ptr + count) > arena->end) return NULL;

  void *ret = (void *)((uintptr_t)arena->base + arena->ptr);
  arena->ptr += count;

  return ret;
}

ARENA_DEF size_t arena_left(Arena *arena)
{
  if (!arena) return 0;
  
  return (arena->end - arena->ptr);
}

ARENA_DEF size_t arena_used(Arena *arena)
{
  if (!arena) return 0;

  return (arena->ptr);
}

#endif //!ARENA_IMPL