#ifndef YOKAI_TEST_H
#define YOKAI_TEST_H

#include <stdio.h>
#include <stdlib.h>

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) static void test_##name(void)

#define RUN_TEST(name) do { \
    tests_run++; \
    printf("  %s... ", #name); \
    test_##name(); \
    tests_passed++; \
    printf("PASSED\n"); \
} while(0)

#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("FAILED\n    %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        exit(1); \
    } \
} while(0)

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_NE(a, b) ASSERT((a) != (b))
#define ASSERT_NULL(ptr) ASSERT((ptr) == NULL)
#define ASSERT_NOT_NULL(ptr) ASSERT((ptr) != NULL)
#define ASSERT_STR_EQ(a, b) ASSERT(strcmp((a), (b)) == 0)

#define TEST_MAIN_BEGIN() int main(void) { \
    printf("Running tests...\n");

#define TEST_MAIN_END() \
    printf("\n%d/%d tests passed\n", tests_passed, tests_run); \
    return tests_run == tests_passed ? 0 : 1; \
}

#endif // YOKAI_TEST_H

