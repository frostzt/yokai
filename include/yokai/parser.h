#ifndef YOKAI_PARSER_H
#define YOKAI_PARSER_H

#include "yokai/ast.h"
#include "yokai/lexer.h"
#include "yokai/token.h"

typedef struct Parser {
  Lexer *lexer;
  Token current_token;
  Token peek_token;
  char **errors;
  size_t error_count;
} Parser;

/* reads the next token into the parser */
void p_next_token(Parser *p);

/* initializes the parser with current and peek token */
void parser_init(Parser *p, Lexer *l);

/* check if the current token type is the same as provided */
bool current_token_is(Parser *p, TokenType ttype);

/* check if the peek token type is the same as provided */
bool peek_token_is(Parser *p, TokenType ttype);

/* moves ahead if the peek token matches the provided one */
bool expect_peek(Parser *p, TokenType ttype);

void peek_error(Parser *p, TokenType ttype);

/*----------------------------------------------------------------
 *  Core parser methods
 *----------------------------------------------------------------*/

/* parses a "let" statement */
LetStatement *parse_let_statement(Parser *p, Arena *arena);

/* parses a statement and returns */
void *parse_statement(Parser *p, Arena *arena);

/* traverses the entire program */
Program *parse_program(Parser *p, Arena *arena);

#endif // YOKAI_PARSER_H
