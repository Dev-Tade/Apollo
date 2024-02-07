#define HASHTABLE_IMPLEMENTATION
#include "../hashtable.h"
#include <stdio.h>

// UNCOMMENT TO USE DEBUG ALLOCATOR
#undef HASHTABLE_ALLOC
#define HASHTABLE_ALLOC(size) debug_malloc(size)
#undef HASHTABLE_FREE
#define HASHTABLE_FREE(ptr) debug_free(ptr)

void *debug_malloc(size_t size) {
  void *ret = malloc(size);
  printf("+ %p #%llu\n", ret, size);
  return ret;
}

void *debug_free(void *block) {
  printf("- %p\n", block);
  free(block);
}

typedef struct {
  int *data;
  char idx;
} sample_struct;

HASHTABLE_IMPL(sample_struct);

int djb2_hash(size_t cap, char *key) {
  int hash = 5381;
  while (*key != 0) {
    hash = ((hash << 5) + hash) + (*key);
    key++;
  }

  return hash % cap;
}

int main() {
  HashTable(sample_struct) data = {0};

  hashtable_init(sample_struct)(&data, 8, djb2_hash);

  hashtable_put(sample_struct)(&data, "foo", (sample_struct){NULL, 2});
  hashtable_put(sample_struct)(&data, "bar", (sample_struct){NULL, 3});

  for (int i=0; i<data.capacity; i++) {
    HashTable_KVP(sample_struct) *current = &(data.items[i]);
    while (current != NULL) {
      printf("%s -> %d\n", current->key, current->value.data);
      current = current->next;
    }
  }

  HashTable_KVP(sample_struct) foo = hashtable_get(sample_struct)(&data, "foo");
  HashTable_KVP(sample_struct) bar = hashtable_del(sample_struct)(&data, "bar");

  printf("%s -> %c\n", foo.key, foo.value.idx);
  printf("%s -> %c\n", bar.key, bar.value.idx);

  for (int i=0; i<data.capacity; i++) {
    HashTable_KVP(sample_struct) *current = &(data.items[i]);
    while (current != NULL) {
      printf("%s -> %d\n", current->key, current->value.data);
      current = current->next;
    }
  }

  hashtable_free(sample_struct)(&data);
  return 0;
}