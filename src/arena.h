#ifndef ARENA_H
#define ARENA_H

#include "common.h"

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
  @param pArena: Pointer to an arena.
  @param size: Amount of bytes for the arena.
  @return `false`: On init failed
  (NULL pArena or Failed to allocate).
*/
APOLLO_DEF bool arena_init(Arena *pArena, size_t size);

/*
  -- Deallocate memory of the Arena --
  @param pArena: Pointer to an arena.
*/
APOLLO_DEF void arena_free(Arena *pArena);

/*
  -- Get memory from the Arena --
  @param pArena: Pointer to an Arena
  @param count: Amount of bytes to reserve
  @return `pointer` to block of reserved memory,
  NULL if no more space is avilable in the Arena
  or NULL pArena
*/
APOLLO_DEF void *arena_reserve(Arena *pArena, size_t count);

/*
  -- Get memory left in the Arena --
  @param pArena: Pointer to an Arena
  @return `Number` of bytes left in arena,
  zero if Arena is NULL
*/
APOLLO_DEF size_t arena_left(Arena *pArena);

/*
  -- Get memory used in the Arena --
  @param pArena: Pointer to an Arena
  @return `Number` of bytes used in arena,
  zero if Arena is NULL
*/
APOLLO_DEF size_t arena_used(Arena *pArena);

#endif //!ARENA_H

#define ARENA_IMPL
#ifdef ARENA_IMPL

#define APOLLO_IMPL
#include "common.h"

APOLLO_DEF bool arena_init(Arena *pArena, size_t size)
{
  if (pArena == NULL) return false;

  pArena->base = APOLLO_ALLOC(size);
  if (pArena->base == NULL) return false;

  APOLLO_MEMSET(pArena->base, 0, pArena->end);
  pArena->end = size;
  pArena->ptr = 0;

  return true;
}

APOLLO_DEF void arena_free(Arena *pArena)
{
  APOLLO_FREE(pArena->base);
}

APOLLO_DEF void *arena_reserve(Arena *pArena, size_t count)
{
  if (pArena == NULL) return NULL;
  if ((pArena->ptr + count) >= pArena->end) return NULL;

  void *ret = (void *)((uintptr_t)pArena->base + pArena->ptr);
  pArena->ptr += count;

  return ret;
}

APOLLO_DEF size_t arena_left(Arena *pArena)
{
  if (pArena == NULL) return 0;
  
  return (pArena->end - pArena->ptr);
}

APOLLO_DEF size_t arena_used(Arena *pArena)
{
  if (pArena == NULL) return 0;

  return (pArena->ptr);
}

#endif //!ARENA_IMPL