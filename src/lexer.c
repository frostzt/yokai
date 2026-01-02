#include <stdlib.h>

#include "yokai/lexer.h"
#include "yokai/str.h"
#include "yokai/token.h"

bool is_digit(char ch) {
  return ch >= '0' && ch <= '9';
}

bool is_letter(char ch) {
  return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}

Token token_make(TokenType type, const char *start, size_t len) {
  Token token = {.type = type, .literal = (StrView){start, len}};
  return token;
}

void read_char(Lexer *lexer) {
  if (lexer->read_position >= lexer->input.len) {
    lexer->ch = 0;
  } else {
    lexer->ch = lexer->input.data[lexer->read_position];
  }

  lexer->position = lexer->read_position;
  lexer->read_position++;
}

char peek_char(Lexer *lexer) {
  if (lexer->read_position >= lexer->input.len) {
    return 0;
  } else {
    return lexer->input.data[lexer->read_position];
  }
}

void skip_whitespace(Lexer *lexer) {
  while (lexer->ch == ' ' || lexer->ch == '\t' || lexer->ch == '\n' || lexer->ch == '\r') {
    read_char(lexer);
  }
}

NumberLex read_number(Lexer *lexer) {
  NumberLex lexed;
  lexed.is_float = false;
  size_t pos = lexer->position;

  size_t dot_counter = 0;
  while (is_digit(lexer->ch) || lexer->ch == '.') {
    if (lexer->ch == '.') {
      lexed.is_float = true;
      dot_counter++;
    }

    if (dot_counter > 1) { lexed.is_invalid = true; }
    read_char(lexer);
  }

  StrView result = {.data = lexer->input.data + pos, .len = lexer->position - pos};
  lexed.number = result;

  return lexed;
}

StrView read_identifier(Lexer *lexer) {
  size_t pos = lexer->position;
  while (is_letter(lexer->ch)) {
    read_char(lexer);
  }

  StrView result = {.data = lexer->input.data + pos, .len = lexer->position - pos};
  return result;
}

Token next_token(Lexer *lexer) {
  Token token;
  skip_whitespace(lexer);

  switch (lexer->ch) {
  case '(': {
    token = token_make(TOK_LPAREN, lexer->input.data + lexer->position, 1);
    break;
  }

  case ')': {
    token = token_make(TOK_RPAREN, lexer->input.data + lexer->position, 1);
    break;
  }

  case '{': {
    token = token_make(TOK_LBRACE, lexer->input.data + lexer->position, 1);
    break;
  }

  case '}': {
    token = token_make(TOK_RBRACE, lexer->input.data + lexer->position, 1);
    break;
  }

  case ';': {
    token = token_make(TOK_SEMICOLON, lexer->input.data + lexer->position, 1);
    break;
  }

  case ',': {
    token = token_make(TOK_COMMA, lexer->input.data + lexer->position, 1);
    break;
  }

  case '+': {
    token = token_make(TOK_PLUS, lexer->input.data + lexer->position, 1);
    break;
  }

  case '-': {
    token = token_make(TOK_MINUS, lexer->input.data + lexer->position, 1);
    break;
  }

  case '*': {
    token = token_make(TOK_ASTERISK, lexer->input.data + lexer->position, 1);
    break;
  }

  case '/': {
    token = token_make(TOK_SLASH, lexer->input.data + lexer->position, 1);
    break;
  }

  case '<': {
    token = token_make(TOK_LT, lexer->input.data + lexer->position, 1);
    break;
  }

  case '>': {
    token = token_make(TOK_GT, lexer->input.data + lexer->position, 1);
    break;
  }

  case '!': {
    char nextPeekedChar = peek_char(lexer);
    if (nextPeekedChar == '=') {
      read_char(lexer);
      token = token_make(TOK_NOT_EQ, lexer->input.data + lexer->position, 2);
    } else {
      token = token_make(TOK_BANG, lexer->input.data + lexer->position, 1);
    }

    break;
  }

  case '=': {
    char nextPeekedChar = peek_char(lexer);
    if (nextPeekedChar == '=') {
      read_char(lexer);
      token = token_make(TOK_EQ, lexer->input.data + lexer->position, 2);
    } else {
      token = token_make(TOK_ASSIGN, lexer->input.data + lexer->position, 1);
    }

    break;
  }

  case 0: {
    token = token_make(TOK_EOF, "", 0);
    break;
  }

  default: {
    if (is_letter(lexer->ch)) {
      token.literal = read_identifier(lexer);
      token.type = lookup_ident(token.literal);
      return token;
    } else if (is_digit(lexer->ch)) {
      NumberLex data_read = read_number(lexer);
      token.literal = data_read.number;

      if (data_read.is_float) {
        token.type = TOK_FLOAT;
      } else if (data_read.is_invalid) {
        token.type = TOK_ILLEGAL;
      } else {
        token.type = TOK_INT;
      }

      return token;
    } else {
      token = token_make(TOK_ILLEGAL, lexer->input.data + lexer->position, 1);
    }
  }
  }

  read_char(lexer);
  return token;
}
