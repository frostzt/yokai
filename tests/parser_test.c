#include <_abort.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "test.h"
#include "yokai/ast.h"
#include "yokai/lexer.h"
#include "yokai/parser.h"

typedef struct ParserTestStruct {
  char *expected_identifier;
} ParserTestStruct;

bool test_let_statement(Statement *stmt, char *name) {
  ASSERT_EQ(stmt->kind, STMT_LET, "token is not of STMT_LET kind");
  ASSERT(strncmp(stmt_token_literal(stmt), "let", 3) == 0, "stmt not let");

  LetStatement *letStatement = (LetStatement *)stmt;
  ASSERT(strcmp(letStatement->name->value.data, name), "expected name does not match");
  return true;
}

void check_parser_errors(Parser *p) {
  char **errors = p__errors(p);
  if (p->error_count == 0) { return; }

  printf("parser encountered %ld errors\n", p->error_count);
  for (size_t i = 0; i < p->error_count; i++) {
    char *err_msg = errors[i];
    printf("parser error: %s\n", err_msg);
  }

  abort();
}

TEST(parser_parses_let_statement) {
  Arena arena = arena_create(512);

  const char *input_raw = "let x = 5; \
                           let y = 10; \
                           let sourav = 84239823; \
                          ";

  StrView input = {.data = input_raw, .len = strlen(input_raw)};
  /* create a new lexer, this internally skips one token */
  Lexer lexer = {.input = input, .position = 0, .read_position = 0, .ch = 0};
  read_char(&lexer);
  /* create a new parser, this internally skips two tokens */
  Parser parser = {.lexer = &lexer};
  parser_init(&parser, &lexer, &arena);
  Program *program = parse_program(&parser, &arena);
  check_parser_errors(&parser);
  ASSERT_NOT_NULL(program, "parse_program returned NULL");
  ASSERT_EQ(program->stmt_count, 3, "program statements do not contain 3 statements");

  ParserTestStruct tests[] = {{.expected_identifier = "x"},
                              {.expected_identifier = "y"},
                              {.expected_identifier = "sourav"}};

  size_t length = sizeof(tests) / sizeof(tests[0]);

  for (size_t i = 0; i < length; i++) {
    ParserTestStruct current_case = tests[i];
    Statement *stmt = program->statements[i];
    ASSERT(test_let_statement(stmt, current_case.expected_identifier),
           "failed to validate let statement");
  }
}

TEST(parser_parses_return_statement) {
  Arena arena = arena_create(512);

  const char *input_raw = "return 5; \
                           return 10; \
                           return 84239823; \
                          ";

  StrView input = {.data = input_raw, .len = strlen(input_raw)};
  /* create a new lexer, this internally skips one token */
  Lexer lexer = {.input = input, .position = 0, .read_position = 0, .ch = 0};
  read_char(&lexer);
  /* create a new parser, this internally skips two tokens */
  Parser parser = {.lexer = &lexer};
  parser_init(&parser, &lexer, &arena);
  Program *program = parse_program(&parser, &arena);
  check_parser_errors(&parser);
  ASSERT_NOT_NULL(program, "parse_program returned NULL");
  ASSERT_EQ(program->stmt_count, 3, "program statements do not contain 3 statements");

  for (size_t i = 0; i < program->stmt_count; i++) {
    Statement *stmt = program->statements[i];
    ASSERT_EQ(stmt->kind, STMT_RETURN, "token is not of STMT_RETURN kind");
    ASSERT(strncmp(stmt_token_literal(stmt), "return", 6) == 0, "stmt not return");
  }
}
