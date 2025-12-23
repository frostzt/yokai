#ifndef YOKAI_LEXER_H
#define YOKAI_LEXER_H

#include <stdbool.h>

#include "yokai/str.h"
#include "yokai/token.h"

typedef struct Lexer {
  StrView input;
  size_t position;
  size_t read_position;
  char ch;
} Lexer;

/* read next character and advance read position */
void read_char(Lexer *lexer);

/* peek and look the next char in line */
char peek_char(Lexer *lexer);

/* check the current charactera and skip if its a whitespace */
void skip_whitespace(Lexer *lexer);

/* read the character as number till the end */
StrView read_number(Lexer *lexer);

/* read the character as an identifier till the end */
StrView read_identifier(Lexer *lexer);

/* read the next token, yeah it does what you expect it to do */
Token next_token(Lexer *lexer);

#endif // YOKAI_LEXER_H
