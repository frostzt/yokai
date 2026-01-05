#include <string.h>

#include "test.h"
#include "yokai/arena.h"

TEST(create_destroy) {
  Arena ar = arena_create(1024);
  arena_destroy(&ar);
}

TEST(arena_alloc_alignment) {
  Arena ar = arena_create(64);

  void *p1 = arena_alloc(&ar, 1, 8);
  ASSERT(((uintptr_t)p1 % 8) == 0, "bad align");

  void *p2 = arena_alloc(&ar, 1, 16);
  ASSERT(((uintptr_t)p2 % 8) == 0, "bad align");

  arena_destroy(&ar);
}

TEST(arena_alloc_many) {
  Arena ar = arena_create(64);

  for (int i = 0; i < 50; i++) {
    void *p = arena_alloc(&ar, 1, 32);
    ASSERT(((uintptr_t)p % 8) == 0, "bad align");
  }

  arena_destroy(&ar);
}

TEST(arena_alloc_growth) {
  Arena ar = arena_create(32);

  void *a = arena_alloc(&ar, 31, 8);
  void *b = arena_alloc(&ar, 24, 8);

  ASSERT(a != NULL, "a is null");
  ASSERT(b != NULL, "b is null");
  ASSERT(ar.head != NULL, "arena head is null");
  ASSERT(ar.head->next != NULL, "arena head failed to grow");

  arena_destroy(&ar);
}

TEST(arena_reset) {
  Arena ar = arena_create(64);
  for (int i = 0; i < 10; i++) {
    void *p = arena_alloc(&ar, 1, 32);
    ASSERT(((uintptr_t)p % 8) == 0, "bad align");
  }

  arena_reset(&ar);

  ASSERT(ar.head->next == NULL, "arena not clear");

  arena_destroy(&ar);
}
