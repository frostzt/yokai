#include "test.h"
#include "test_registry.h"

// test funcs
#define X(name) void test_##name(void);
ALL_TESTS
#undef X

TEST_MAIN_BEGIN()
#define X(name) RUN_TEST(name);
ALL_TESTS
#undef X
TEST_MAIN_END()
