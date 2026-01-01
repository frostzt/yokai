#ifndef YOKAI_TEST_REGISTRY_H
#define YOKAI_TEST_REGISTRY_H

// tests
#define ALL_TESTS                                                                                  \
  X(create_destroy)                                                                                \
  X(arena_alloc_alignment)                                                                         \
  X(arena_alloc_many)                                                                              \
  X(arena_alloc_growth)                                                                            \
  X(arena_reset)                                                                                   \
  X(lexer_full_token)                                                                              \
  X(ast_simple_stmt)

#endif // YOKAI_TEST_REGISTRY_H
