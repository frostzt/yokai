#include <assert.h>
#include <string.h>

#include "test.h"
#include "yokai/lexer.h"
#include "yokai/token.h"

typedef struct TestStruct {
  TokenType expected_type;
  char *expected_literal;
} TestStruct;

TEST(lexer_full_token) {
  const char *input_raw = "let five = 5; \
                       let ten = 10.9; \
                       \
                       let add = fn(x, y) { \
                         x + y; \
                       }; \
                       \
                       let result = add(five, ten); \
                       !-/*5; \
                       5 < 10 > 5; \
                       \
                       if (5 < 10) { \
                        return true; \
                       } else { \
                        return false; \
                       } \
                       \
                       10 == 10; \
                       10 != 9;";

  StrView input = {.data = input_raw, .len = strlen(input_raw)};

  TestStruct tests[] = {
      // 1st line
      {.expected_type = TOK_LET, .expected_literal = "let"},
      {.expected_type = TOK_IDENT, .expected_literal = "five"},
      {.expected_type = TOK_ASSIGN, .expected_literal = "="},
      {.expected_type = TOK_INT, .expected_literal = "5"},
      {.expected_type = TOK_SEMICOLON, .expected_literal = ";"},

      // 2nd line
      {.expected_type = TOK_LET, .expected_literal = "let"},
      {.expected_type = TOK_IDENT, .expected_literal = "ten"},
      {.expected_type = TOK_ASSIGN, .expected_literal = "="},
      {.expected_type = TOK_FLOAT, .expected_literal = "10.9"},
      {.expected_type = TOK_SEMICOLON, .expected_literal = ";"},

      // 3rd line
      {.expected_type = TOK_LET, .expected_literal = "let"},
      {.expected_type = TOK_IDENT, .expected_literal = "add"},
      {.expected_type = TOK_ASSIGN, .expected_literal = "="},
      {.expected_type = TOK_FUNCTION, .expected_literal = "fn"},
      {.expected_type = TOK_LPAREN, .expected_literal = "("},
      {.expected_type = TOK_IDENT, .expected_literal = "x"},
      {.expected_type = TOK_COMMA, .expected_literal = ","},
      {.expected_type = TOK_IDENT, .expected_literal = "y"},
      {.expected_type = TOK_RPAREN, .expected_literal = ")"},
      {.expected_type = TOK_LBRACE, .expected_literal = "{"},

      // 4th line
      {.expected_type = TOK_IDENT, .expected_literal = "x"},
      {.expected_type = TOK_PLUS, .expected_literal = "+"},
      {.expected_type = TOK_IDENT, .expected_literal = "y"},
      {.expected_type = TOK_SEMICOLON, .expected_literal = ";"},

      // 5th line
      {.expected_type = TOK_RBRACE, .expected_literal = "}"},
      {.expected_type = TOK_SEMICOLON, .expected_literal = ";"},

      // 6th line
      {.expected_type = TOK_LET, .expected_literal = "let"},
      {.expected_type = TOK_IDENT, .expected_literal = "result"},
      {.expected_type = TOK_ASSIGN, .expected_literal = "="},
      {.expected_type = TOK_IDENT, .expected_literal = "add"},
      {.expected_type = TOK_LPAREN, .expected_literal = "("},
      {.expected_type = TOK_IDENT, .expected_literal = "five"},
      {.expected_type = TOK_COMMA, .expected_literal = ","},
      {.expected_type = TOK_IDENT, .expected_literal = "ten"},
      {.expected_type = TOK_RPAREN, .expected_literal = ")"},
      {.expected_type = TOK_SEMICOLON, .expected_literal = ";"},

      // 7th line
      {.expected_type = TOK_BANG, .expected_literal = "!"},
      {.expected_type = TOK_MINUS, .expected_literal = "-"},
      {.expected_type = TOK_SLASH, .expected_literal = "/"},
      {.expected_type = TOK_ASTERISK, .expected_literal = "*"},
      {.expected_type = TOK_INT, .expected_literal = "5"},
      {.expected_type = TOK_SEMICOLON, .expected_literal = ";"},

      // 8th line
      {.expected_type = TOK_INT, .expected_literal = "5"},
      {.expected_type = TOK_LT, .expected_literal = "<"},
      {.expected_type = TOK_INT, .expected_literal = "10"},
      {.expected_type = TOK_GT, .expected_literal = ">"},
      {.expected_type = TOK_INT, .expected_literal = "5"},
      {.expected_type = TOK_SEMICOLON, .expected_literal = ";"},

      // 9th line
      {.expected_type = TOK_IF, .expected_literal = "if"},
      {.expected_type = TOK_LPAREN, .expected_literal = "("},
      {.expected_type = TOK_INT, .expected_literal = "5"},
      {.expected_type = TOK_LT, .expected_literal = "<"},
      {.expected_type = TOK_INT, .expected_literal = "10"},
      {.expected_type = TOK_RPAREN, .expected_literal = ")"},
      {.expected_type = TOK_LBRACE, .expected_literal = "{"},

      // 10th line
      {.expected_type = TOK_RETURN, .expected_literal = "return"},
      {.expected_type = TOK_TRUE, .expected_literal = "true"},
      {.expected_type = TOK_SEMICOLON, .expected_literal = ";"},

      // 11th line
      {.expected_type = TOK_RBRACE, .expected_literal = "}"},
      {.expected_type = TOK_ELSE, .expected_literal = "else"},
      {.expected_type = TOK_LBRACE, .expected_literal = "{"},

      // 12th line
      {.expected_type = TOK_RETURN, .expected_literal = "return"},
      {.expected_type = TOK_FALSE, .expected_literal = "false"},
      {.expected_type = TOK_SEMICOLON, .expected_literal = ";"},

      // 13th line
      {.expected_type = TOK_RBRACE, .expected_literal = "}"},

      // 14th line
      {.expected_type = TOK_INT, .expected_literal = "10"},
      {.expected_type = TOK_EQ, .expected_literal = "=="},
      {.expected_type = TOK_INT, .expected_literal = "10"},
      {.expected_type = TOK_SEMICOLON, .expected_literal = ";"},

      // 15th line
      {.expected_type = TOK_INT, .expected_literal = "10"},
      {.expected_type = TOK_NOT_EQ, .expected_literal = "!="},
      {.expected_type = TOK_INT, .expected_literal = "9"},
      {.expected_type = TOK_SEMICOLON, .expected_literal = ";"},
  };

  Lexer lexer = {.input = input, .position = 0, .read_position = 0, .ch = 0};
  read_char(&lexer);
  size_t length = sizeof(tests) / sizeof(tests[0]);

  for (size_t i = 0; i < length; i++) {
    TestStruct current_case = tests[i];
    Token tok = next_token(&lexer);
    assert(tok.type == current_case.expected_type);
  }
}
