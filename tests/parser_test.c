#include <_abort.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "test.h"
#include "yokai/arena.h"
#include "yokai/ast.h"
#include "yokai/lexer.h"
#include "yokai/parser.h"
#include "yokai/str.h"

typedef struct ParserTestStruct {
  char *expected_identifier;
} ParserTestStruct;

bool test_let_statement(Statement *stmt, char *name) {
  ASSERT_EQ(stmt->kind, STMT_LET, "token is not of STMT_LET kind");
  ASSERT(strncmp(stmt_token_literal(stmt), "let", 3) == 0, "stmt not let");

  ASSERT(strcmp(stmt->as.stmt_let.name->as.expr_ident.value.data, name),
         "expected name does not match");
  return true;
}

bool test_integer_literal(Expression *expr, int64_t value) {
  ASSERT_EQ(expr->kind, EXPR_INT, "expr is not an integer literal expression");
  ASSERT_EQ(expr->as.expr_int_literal.value, value, "integer value mismatch");
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

TEST(parser__parses_let_statement) {
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

TEST(parser__parses_return_statement) {
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

TEST(parser__simple_identifier_expression) {
  Arena arena = arena_create(128);

  const char *input_raw = "foobar;";

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
  ASSERT_EQ(program->stmt_count, 1, "program statements does not contain 1 statement");

  Statement *stmt = program->statements[0];
  ASSERT_EQ(stmt->kind, STMT_EXPR, "not an expression statement");

  Expression *ident = stmt->as.stmt_expr.expr;
  ASSERT(sv_eq_cstr(ident->as.expr_ident.value, "foobar"), "invalid identifier");
  ASSERT(sv_eq_cstr(ident->token.literal, "foobar"), "invalid token literal");
}

TEST(parser__integer_literal_expression) {
  Arena arena = arena_create(128);

  const char *input_raw = "52;";

  /* setup parser */
  StrView input = {.data = input_raw, .len = strlen(input_raw)};
  Lexer lexer = {.input = input, .position = 0, .read_position = 0, .ch = 0};
  read_char(&lexer);
  Parser parser = {.lexer = &lexer};
  parser_init(&parser, &lexer, &arena);
  Program *program = parse_program(&parser, &arena);
  check_parser_errors(&parser);

  ASSERT_NOT_NULL(program, "parse_program returned NULL");
  ASSERT_EQ(program->stmt_count, 1, "program statements does not contain 1 statement");

  Statement *stmt = program->statements[0];
  ASSERT_EQ(stmt->kind, STMT_EXPR, "statement is not an expression statement");

  ASSERT_EQ(stmt->as.stmt_expr.expr->kind, EXPR_INT, "statement expression not an integer kind");
  ASSERT_EQ(stmt->as.stmt_expr.expr->as.expr_int_literal.value, 52, "literal value not 52");
  ASSERT(sv_eq_cstr(stmt->as.stmt_expr.expr->token.literal, "52"), "literal not 52");
}

TEST(parser__float_literal_expression) {
  Arena arena = arena_create(128);

  const char *input_raw = "52.64;";

  /* setup parser */
  StrView input = {.data = input_raw, .len = strlen(input_raw)};
  Lexer lexer = {.input = input, .position = 0, .read_position = 0, .ch = 0};
  read_char(&lexer);
  Parser parser = {.lexer = &lexer};
  parser_init(&parser, &lexer, &arena);
  Program *program = parse_program(&parser, &arena);
  check_parser_errors(&parser);

  ASSERT_NOT_NULL(program, "parse_program returned NULL");
  ASSERT_EQ(program->stmt_count, 1, "program statements does not contain 1 statement");

  Statement *stmt = program->statements[0];
  ASSERT_EQ(stmt->kind, STMT_EXPR, "statement is not an expression statement");

  ASSERT_EQ(stmt->as.stmt_expr.expr->kind, EXPR_FLOAT, "statement expression not a float kind");
  ASSERT_EQ(stmt->as.stmt_expr.expr->as.expr_float_literal.value, 52.64, "literal value not 52.64");
  ASSERT(sv_eq_cstr(stmt->as.stmt_expr.expr->token.literal, "52.64"), "literal not 52.64");
}

TEST(parser__parsing_prefix_expression) {
  Arena arena = arena_create(128);

  struct PrefixTest {
    char *input;
    char *operator;
    int64_t integer_value;
  };

  struct PrefixTest tests[] = {
      {.input = "!5", .operator = "!", .integer_value = 5},
      {.input = "-15", .operator = "-", .integer_value = 15},
  };

  size_t length = sizeof(tests) / sizeof(tests[0]);
  for (size_t i = 0; i < length; i++) {
    /* construct the logic out of current case's input */
    struct PrefixTest current_case = tests[i];
    StrView input = {.data = current_case.input, .len = strlen(current_case.input)};
    Lexer lexer = {.input = input, .position = 0, .read_position = 0, .ch = 0};
    read_char(&lexer);
    Parser parser = {.lexer = &lexer};
    parser_init(&parser, &lexer, &arena);
    Program *program = parse_program(&parser, &arena);
    check_parser_errors(&parser);

    ASSERT_NOT_NULL(program, "parse_program returned NULL");
    ASSERT_EQ(program->stmt_count, 1, "program statements does not contain 1 statement");

    Statement *stmt = program->statements[0];
    ASSERT_EQ(stmt->kind, STMT_EXPR, "not an expression statement");
    Expression *stmt_expr = stmt->as.stmt_expr.expr;
    ASSERT_EQ(stmt_expr->kind, EXPR_PREFIX, "stmt expression not prefix expression");
    ASSERT(sv_eq_cstr(stmt_expr->as.expr_prefix.op, current_case.operator), "operator mismatch");
    ASSERT(test_integer_literal(stmt_expr->as.expr_prefix.right, current_case.integer_value),
           "invalid integer literal value");
  }
}

TEST(parser__parsing_infix_expression) {
  Arena arena = arena_create(128);

  struct PrefixTest {
    char *input;
    int64_t left_value;
    char *operator;
    int64_t right_value;
  };

  struct PrefixTest tests[] = {
      {.input = "5 + 5", .operator = "+", .left_value = 5, .right_value = 5},
      {.input = "5 - 5", .operator = "-", .left_value = 5, .right_value = 5},
      {.input = "5 * 5", .operator = "*", .left_value = 5, .right_value = 5},
      {.input = "5 / 5", .operator = "/", .left_value = 5, .right_value = 5},
      {.input = "5 > 5", .operator = ">", .left_value = 5, .right_value = 5},
      {.input = "5 < 5", .operator = "<", .left_value = 5, .right_value = 5},
      {.input = "5 == 5", .operator = "==", .left_value = 5, .right_value = 5},
      {.input = "5 != 5", .operator = "!=", .left_value = 5, .right_value = 5},
  };

  size_t length = sizeof(tests) / sizeof(tests[0]);
  for (size_t i = 0; i < length; i++) {
    /* construct the logic out of current case's input */
    struct PrefixTest current_case = tests[i];
    StrView input = {.data = current_case.input, .len = strlen(current_case.input)};
    Lexer lexer = {.input = input, .position = 0, .read_position = 0, .ch = 0};
    read_char(&lexer);
    Parser parser = {.lexer = &lexer};
    parser_init(&parser, &lexer, &arena);
    Program *program = parse_program(&parser, &arena);
    check_parser_errors(&parser);

    ASSERT_NOT_NULL(program, "parse_program returned NULL");
    ASSERT_EQ(program->stmt_count, 1, "program statements does not contain 1 statement");

    Statement *stmt = program->statements[0];
    ASSERT_EQ(stmt->kind, STMT_EXPR, "not an expression statement");
    Expression *stmt_expr = stmt->as.stmt_expr.expr;
    ASSERT_EQ(stmt_expr->kind, EXPR_INFIX, "stmt expression not infix expression");
    ASSERT(test_integer_literal(stmt_expr->as.expr_infix.left, current_case.left_value),
           "invalid left value");
    ASSERT(sv_eq_cstr(stmt_expr->as.expr_infix.op, current_case.operator), "operator mismatch");
    ASSERT(test_integer_literal(stmt_expr->as.expr_infix.right, current_case.right_value),
           "invalid right value");
  }
}
