#ifndef YOKAI_TOKEN_H
#define YOKAI_TOKEN_H

#include "yokai/str.h"

typedef enum TokenType {
  TOK_ILLEGAL,
  TOK_EOF,

  // identifiers and literals
  TOK_IDENT,
  TOK_INT,

  // comparision operators
  TOK_EQ,
  TOK_NOT_EQ,

  TOK_SEMICOLON,
  TOK_COMMA,
  TOK_ASSIGN,
  TOK_PLUS,
  TOK_MINUS,
  TOK_BANG,
  TOK_ASTERISK,
  TOK_SLASH,
  TOK_LT,
  TOK_GT,

  TOK_LPAREN,
  TOK_RPAREN,
  TOK_LBRACE,
  TOK_RBRACE,

  // keywords
  TOK_FUNCTION,
  TOK_LET,
  TOK_TRUE,
  TOK_FALSE,
  TOK_IF,
  TOK_ELSE,
  TOK_RETURN,
} TokenType;

typedef struct Token {
  TokenType type;
  StrView literal;
} Token;

/* Lookup a token type with its identifier */
TokenType lookup_ident(StrView ident);

const char *token_type_name(TokenType t);

#endif // YOKAI_TOKEN_H
