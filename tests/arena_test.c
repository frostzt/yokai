#include <string.h>

#include "test.h"
#include "yokai/arena.h"

TEST(create_destory) {
  Arena ar = arena_create(1024);
  arena_destroy(&ar);
}

TEST(arena_alloc_alignment) {
  Arena ar = arena_create(64);

  void *p1 = arena_alloc(&ar, 1, 8);
  ASSERT(((uintptr_t)p1 % 8) == 0);

  void *p2 = arena_alloc(&ar, 1, 16);
  ASSERT(((uintptr_t)p2 % 8) == 0);

  arena_destroy(&ar);
}

TEST(arena_alloc_many) {
  Arena ar = arena_create(64);

  for (int i = 0; i < 50; i++) {
    void *p = arena_alloc(&ar, 1, 32);
    ASSERT(((uintptr_t)p % 8) == 0);
  }

  arena_destroy(&ar);
}

TEST(arena_alloc_growth) {
  Arena ar = arena_create(32);

  void *a = arena_alloc(&ar, 31, 8);
  void *b = arena_alloc(&ar, 24, 8);

  ASSERT(a != NULL);
  ASSERT(b != NULL);
  ASSERT(ar.head != NULL);
  ASSERT(ar.head->next != NULL);

  arena_destroy(&ar);
}

TEST(arena_reset) {
  Arena ar = arena_create(64);
  for (int i = 0; i < 10; i++) {
    void *p = arena_alloc(&ar, 1, 32);
    ASSERT(((uintptr_t)p % 8) == 0);
  }

  arena_reset(&ar);

  ASSERT(ar.head->next == NULL);

  arena_destroy(&ar);
}

TEST_MAIN_BEGIN()
  RUN_TEST(create_destory);
  RUN_TEST(arena_alloc_alignment);
  RUN_TEST(arena_alloc_growth);
  RUN_TEST(arena_alloc_many);
  RUN_TEST(arena_reset);
TEST_MAIN_END()
