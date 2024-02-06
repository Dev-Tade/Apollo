#define HASHTABLE_IMPLEMENTATION
#include "../hashtable.h"
#include <stdio.h>

// UNCOMMENT TO USE DEBUG ALLOCATOR
// #undef HASHTABLE_ALLOC
// #define HASHTABLE_ALLOC(size) debug_malloc(size)
// #undef HASHTABLE_FREE
// #define HASHTABLE_FREE(ptr) debug_free(ptr)

// void *debug_malloc(size_t size) {
//   void *ret = malloc(size);
//   printf("+ %p #%llu\n", ret, size);
//   return ret;
// }

// void *debug_free(void *block) {
//   printf("- %p\n", block);
//   free(block);
// }

HASHTABLE_IMPL(int);

int djb2_hash(size_t cap, char *key) {
  int hash = 5381;
  while (*key != 0) {
    hash = ((hash << 5) + hash) + (*key);
    key++;
  }

  return hash % cap;
}

int main() {
  HashTable(int) data = {0};

  hashtable_init(int)(&data, 8, djb2_hash);

  hashtable_put(int)(&data, "foo", 1);
  hashtable_put(int)(&data, "bar", 2);
  hashtable_put(int)(&data, "baz", 3);
  hashtable_put(int)(&data, "zab", 4);
  hashtable_put(int)(&data, "cls", 5);
  hashtable_put(int)(&data, "neg", 6);

  for (int i=0; i<data.capacity; i++) {
    HashTable_KVP(int) *current = &(data.items[i]);
    while (current != NULL) {
      printf("%s -> %d\n", current->key, current->value);
      current = current->next;
    }
  }

  HashTable_KVP(int) foo = hashtable_get(int)(&data, "foo");
  HashTable_KVP(int) bar = hashtable_del(int)(&data, "bar");

  printf("%s -> %f\n", foo.key, foo.value);
  printf("%s -> %f\n", bar.key, bar.value);

  hashtable_free(int)(&data);
  return 0;
}