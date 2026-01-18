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
  X(ast_simple_stmt)                                                                               \
  X(parser__parses_let_statement)                                                                   \
  X(parser__parses_return_statement)                                                                \
  X(strbuf_through_check)

#endif // YOKAI_TEST_REGISTRY_H
