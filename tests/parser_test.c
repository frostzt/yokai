#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "test.h"
#include "yokai/arena.h"
#include "yokai/ast.h"
#include "yokai/debug.h"
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

bool test_identifier_literal(Expression *expr, char *value) {
  ASSERT_EQ(expr->kind, EXPR_IDENT, "expr is not of identifier kind");
  ASSERT(sv_eq_cstr(expr->as.expr_ident.value, value), "value mismatch");
  return true;
}

bool test_literal_expression(Expression *expr, void *expected) {
  switch (expr->kind) {
  case EXPR_INT: {
    int64_t value = *(int64_t *)expected;
    return test_integer_literal(expr, value);
  }
  case EXPR_IDENT: {
    return test_identifier_literal(expr, expected);
  }
  default: {
    DEBUG_ERROR("Type received is out of scope");
    return false;
  }
  }
}

bool test_infix_expression(Expression *infix_expr, void *left, char *operator, void *right) {
  ASSERT_EQ(infix_expr->kind, EXPR_INFIX, "expression kind mismatch");
  ASSERT(test_literal_expression(infix_expr->as.expr_infix.left, left),
         "mismatched left expression types");
  ASSERT(sv_eq_cstr(infix_expr->as.expr_infix.op, operator), "mismatched op types");
  ASSERT(test_literal_expression(infix_expr->as.expr_infix.right, right),
         "mismatched right expression types");
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

TEST(parser__boolean_literal_expression) {
  Arena arena = arena_create(128);

  const char *input_raw = "true;";

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

  ASSERT_EQ(stmt->as.stmt_expr.expr->kind, EXPR_BOOLEAN,
            "statement expression not an integer kind");
  ASSERT_EQ(stmt->as.stmt_expr.expr->as.expr_bool_literal.value, true, "literal value not true");
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

  struct InfixTest {
    char *input;
    int64_t left_value;
    char *operator;
    int64_t right_value;
  };

  struct InfixTest tests[] = {
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
    struct InfixTest current_case = tests[i];
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
    ASSERT(test_infix_expression(stmt_expr, &current_case.left_value, current_case.operator,
                                 &current_case.right_value),
           "failed to test infix expression");
  }
}

TEST(parser__parsing_complex_expressions) {
  Arena arena = arena_create(128);

  struct ComplexExpressionTest {
    char *input;
    char *output;
  };

  struct ComplexExpressionTest tests[] = {
      {.input = "-a * b", .output = "((-a) * b)"},
      {
          .input = "!-a",
          .output = "(!(-a))",
      },
      {
          .input = "a + b + c",
          .output = "((a + b) + c)",
      },
      {
          .input = "a + b - c",
          .output = "((a + b) - c)",
      },
      {
          .input = "a * b * c",
          .output = "((a * b) * c)",
      },
      {
          .input = "a * b / c",
          .output = "((a * b) / c)",
      },
      {
          .input = "a + b / c",
          .output = "(a + (b / c))",
      },
      {
          .input = "a + b * c + d / e - f",
          .output = "(((a + (b * c)) + (d / e)) - f)",
      },
      {
          .input = "3 + 4; -5 * 5",
          .output = "(3 + 4)((-5) * 5)",
      },
      {
          .input = "5 > 4 == 3 < 4",
          .output = "((5 > 4) == (3 < 4))",
      },
      {
          .input = "5 < 4 != 3 > 4",
          .output = "((5 < 4) != (3 > 4))",
      },
      {
          .input = "3 + 4 * 5 == 3 * 1 + 4 * 5",
          .output = "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
      },
      {
          .input = "3 + 4 * 5 == 3 * 1 + 4 * 5",
          .output = "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
      },
  };

  size_t length = sizeof(tests) / sizeof(tests[0]);
  for (size_t i = 0; i < length; i++) {
    struct ComplexExpressionTest current_case = tests[i];
    StrView input = {.data = current_case.input, .len = strlen(current_case.input)};
    Lexer lexer = {.input = input, .position = 0, .read_position = 0, .ch = 0};
    read_char(&lexer);
    Parser parser = {.lexer = &lexer};
    parser_init(&parser, &lexer, &arena);
    Program *program = parse_program(&parser, &arena);
    check_parser_errors(&parser);

    ASSERT_NOT_NULL(program, "parse_program returned NULL");

    /* for program string */
    StrBuf strbuf;
    strbuf.arena = &arena;
    sb__init(&strbuf, 32);

    /* convert the entire program to string */
    ast__prog_to_string(program, &strbuf);

    const char *actual = sb__cstr(&strbuf);
    ASSERT(strcmp(current_case.output, actual) == 0, "program string mismatch");
  }
}
