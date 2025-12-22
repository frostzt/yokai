#include <string.h>

#include "test.h"
#include "yokai/arena.h"

TEST(create_destory) {
  Arena ar = arena_create(1024);
  arena_destroy(&ar);
}

TEST_MAIN_BEGIN()
  RUN_TEST(create_destory);
TEST_MAIN_END()
