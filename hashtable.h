#ifndef HASHTABLE_H
#define HASHTABLE_H

/*
  GENERIC HASHTABLE IMPLEMENTATION IN C FOR APOLLO CODEBASE

  % The hashtable is implemented as as set of macros that implement
    and give the respective type implementation
    the type is only used for the value field
    all keys are of type char *.

  % The hashtable is of static size
    altough can allocate more memory for handling collisions

  % Collisions are handled by chaining

  User defined macros:
    HASHTABLE_ALLOC(size) -> Default: malloc, can be redefined to use another allocator in the same signature of malloc()
    HASHTABLE_FREE(size) -> Default: free, can be redefined to use another allocator in the same signature of free()

  Types:
    % Types with <type> are generated by macros and thus can be getted by a macro

  HashFunction -> int (*)(size_t cap, char *key)

  <type>_HashTable_KVP ->
    struct {
      char *key
      type value
      <type>_HashTable_KVP *next
    }

  <type>_HashTable ->
    struct {
      HashFunction hash; 
      <type>_HashTable_KVP *items;
      size_t capacity;
    }

  Public Macros:
    HASHTABLE_IMPL(type) -> implements functions for a table of specified type
    HASHTABLE_DECL(type) -> declares structures and functions for a table of specified type
    
    HashTable(type) -> returns <type>_HashTable for declaration / cast
    HashTable_KVP(type) -> returns <type>_HashTable_KVP for declaration / cast

    hashtable_init(type) -> returns init function for corresponding type
    hashtable_free(type) -> returns free function for corresponding type

    hashtable_put(type) -> returns put function for corresponding type
    hashtable_get(type) -> returns get function for corresponding type
    hashtable_del(type) -> returns del function for corresponding type

  Private Macros:
    % These macros are used internaly please don't use

    HASHTABLE_IMPL_KVP
    HASHTABLE_IMPL_HT
    HASHTABLE_IMPL_COLLIDES
    HASHTABLE_IMPL_INIT
    HASHTABLE_IMPL_FREE
    HASHTABLE_IMPL_PUT
    HASHTABLE_IMPL_GET
    HASHTABLE_IMPL_DEL

  Functions:
    % Underlying functions for hashtable operations
    
    hashtable_init(<type>_HashTable *table, size_t size, HashFunction hash) ->
      Allocates (size) amount of KVPs in (table), and sets the hashing function to (hash),
      returns 1 on malloc error, 0 on success

    hashtable_free(<type>_HashTable *table) ->
      Deallocates memory used by (table)

    hashtable_put(<type>_HashTable *table, char *key, <type> value) ->
      Inserts / Updates (key) into (table) with value (value),
      returns 1 on malloc error, 0 on succes

    hashtable_get((<type>_HashTable *table, char *key) ->
      Returns a the KVP associated with (key)

    hashtable_del(type) ->
      Returns the KVP associated with (key) and deletes it from the table,
      handles chains
*/

#ifndef APOLLO_DEF
#define APOLLO_DEF extern
#else
#undef APOLLO_DEF
#define APOLLO_DEF extern
#endif

#include <stddef.h>

#define HASHTABLE_ALLOC(size) malloc(size)
#define HASHTABLE_FREE(ptr) free(ptr)

typedef int (*HashFunction)(size_t cap, char *key);

#define HashTable(type) type##_HashTable
#define HashTable_KVP(type) type##_HashTable_KVP

#define HASHTABLE_DECL(type)                                                                     \
typedef struct s_##type##_kvp type##_HashTable_KVP;                                              \
typedef struct s_##type##_ht type##_HashTable;                                                   \
APOLLO_DEF int type##_hashtable_init(HashTable(type) *table, size_t size, HashFunction hash);    \
APOLLO_DEF void type##_hashtable_free(HashTable(type) *table);                                   \
APOLLO_DEF int type##_hashtable_put(HashTable(type) *table, char *key, type value);              \
APOLLO_DEF HashTable_KVP(type) type##_hashtable_get(HashTable(type) *table, char *key);          \
APOLLO_DEF HashTable_KVP(type) type##_hashtable_del(HashTable(type) *table, char *key);          \

#define hashtable_init(type) type##_hashtable_init
#define hashtable_free(type) type##_hashtable_free
#define hashtable_put(type)  type##_hashtable_put
#define hashtable_get(type)  type##_hashtable_get
#define hashtable_del(type)  type##_hashtable_del

#ifdef HASHTABLE_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef APOLLO_DEF
#define APOLLO_DEF static
#else
#undef APOLLO_DEF
#define APOLLO_DEF static
#endif

#define HASHTABLE_IMPL(type)        \
HASHTABLE_IMPL_KVP(type)            \
HASHTABLE_IMPL_HT(type)             \
HASHTABLE_IMPL_COLLIDES(type)       \
HASHTABLE_IMPL_INIT(type)           \
HASHTABLE_IMPL_FREE(type)           \
HASHTABLE_IMPL_PUT(type)            \
HASHTABLE_IMPL_GET(type)            \
HASHTABLE_IMPL_DEL(type)            \


/* INTERNAL MACRO!!!!!, DO NOT USE */
#define HASHTABLE_IMPL_KVP(type)    \
typedef struct s_##type##_kvp {     \
  char *key;                        \
  type value;                       \
  struct s_##type##_kvp *next;      \
} HashTable_KVP(type);              \

/* INTERNAL MACRO!!!!!, DO NOT USE */
#define HASHTABLE_IMPL_HT(type)     \
typedef struct s_##type##_ht {      \
  HashFunction hash;                \
  HashTable_KVP(type) *items;       \
  size_t capacity;                  \
} HashTable(type);                  \

/* INTERNAL MACRO!!!!!, DO NOT USE */
#define HASHTABLE_IMPL_COLLIDES(type)                                                             \
APOLLO_DEF int type##_hashtable_collides(HashTable(type) *table, int hash, char *key) {           \
  if (table->items[hash].key == NULL) return 0;                                                   \
  else if (table->items[hash].key != NULL && strcmp(table->items[hash].key, key) == 0) return 0;  \
  else if (table->items[hash].key != NULL && strcmp(table->items[hash].key, key) != 0) return 1;  \
  else assert(0 && "UNREACHEABLE");                                                               \
}                                                                                                 \

/* INTERNAL MACRO!!!!!, DO NOT USE */
#define HASHTABLE_COLLIDES(type) type##_hashtable_collides

/* INTERNAL MACRO!!!!!, DO NOT USE */
#define HASHTABLE_IMPL_INIT(type)                                                             \
APOLLO_DEF int type##_hashtable_init(HashTable(type) *table, size_t size, HashFunction hash) {\
  table->capacity = size;                                                                     \
  table->hash = hash;                                                                         \
  size_t bs = sizeof(HashTable_KVP(type)) * table->capacity;                                  \
  table->items = (HashTable_KVP(type)*) HASHTABLE_ALLOC(bs);                                  \
  if (table->items == NULL) return 1;                                                         \
  memset(table->items, 0, bs);                                                                \
  return 0;                                                                                   \
}                                                                                             \

/* INTERNAL MACRO!!!!!, DO NOT USE */
#define HASHTABLE_IMPL_FREE(type)                                   \
APOLLO_DEF void type##_hashtable_free(HashTable(type) *table) {     \
  for (int i=0; i<table->capacity; i++) {                           \
    HashTable_KVP(type) *curr = table->items[i].next;               \
    HashTable_KVP(type) *copy = curr;                               \
    while (curr != NULL) {                                          \
      curr = curr->next;          	                                \
      HASHTABLE_FREE(copy);       	                                \
      copy = curr;                	                                \
    }                             	                                \
  }                               	                                \
  HASHTABLE_FREE(table->items);   	                                \
}                                 	                                \

/* INTERNAL MACRO!!!!!, DO NOT USE */
#define HASHTABLE_IMPL_PUT(type)                                                        \
APOLLO_DEF int type##_hashtable_put(HashTable(type) *table, char *key, type value) {    \
  int hash = table->hash(table->capacity, key);                                         \
  if (!HASHTABLE_COLLIDES(type)(table, hash, key)) {                                    \
    HashTable_KVP(type) *next = (table->items[hash].next);                              \
    table->items[hash].key = key;                                                       \
    table->items[hash].value = value;                                                   \
    table->items[hash].next = next == NULL ? NULL : next;                               \
  } else {                                                                              \
    HashTable_KVP(type) *current = &(table->items[hash]);                               \
    while (current->next != NULL) current = current->next;                              \
    current->next = (HashTable_KVP(type)*)HASHTABLE_ALLOC(sizeof(HashTable_KVP(type))); \
    if (current->next == NULL) return 1;                                                \
    current->next->key = key;                                                           \
    current->next->value = value;                                                       \
    current->next->next = NULL;                                                         \
  }                                                                                     \
  return 0;                                                                             \
}                                                                                       \

/* INTERNAL MACRO!!!!!, DO NOT USE */
#define HASHTABLE_IMPL_GET(type)                                                        \
APOLLO_DEF HashTable_KVP(type) type##_hashtable_get(HashTable(type) *table, char *key) {\
  int hash = table->hash(table->capacity, key);                                         \
  if (!HASHTABLE_COLLIDES(type)(table, hash, key)) {                                    \
    return table->items[hash];                                                          \
  } else {                                                                              \
    HashTable_KVP(type) *current = &(table->items[hash]);                               \
    while (strcmp(current->key, key) != 0 && current->next != NULL)                     \
      current = current->next;                                                          \
    return *current;                                                                    \
  }                                                                                     \
}                                                                                       \

/* INTERNAL MACRO!!!!!, DO NOT USE */
#define HASHTABLE_IMPL_DEL(type)                                                        \
APOLLO_DEF HashTable_KVP(type) type##_hashtable_del(HashTable(type) *table, char *key) {\
  int hash = table->hash(table->capacity, key);                                         \
  HashTable_KVP(type) item = {0};                                                       \
  if (!HASHTABLE_COLLIDES(type)(table, hash, key)) {                                    \
    item = table->items[hash];                                                          \
    if (table->items[hash].next == NULL) {                                              \
      table->items[hash].key = NULL;                                                    \
      table->items[hash].value = 0;                                                     \
    } else {                                                                            \
      HashTable_KVP(type) *collision = table->items[hash].next;                         \
      table->items[hash].key = collision->key;                                          \
      table->items[hash].value = collision->value;                                      \
      table->items[hash].next = collision->next;                                        \
      HASHTABLE_FREE(collision);                                                        \
    }                                                                                   \
  } else {                                                                              \
    HashTable_KVP(type) *current = &(table->items[hash]);                               \
    HashTable_KVP(type) *previous = NULL;                                               \
    while (strcmp(current->key, key) != 0 && current->next != NULL) {                   \
      previous = current;                                                               \
      current = current->next;                                                          \
    }                                                                                   \
    if (current != NULL) {                                                              \
      item = *current;                                                                  \
      previous->next = current->next;                                                   \
      HASHTABLE_FREE(current);                                                          \
    }                                                                                   \
  }                                                                                     \
  return item;                                                                          \
}                                                                                       \

#endif
#endif