/*
 * parser.h - Recursive descent parser for Yokai
 *
 * Author: frostzt
 * Date: 2026-01-05
 */

#ifndef YOKAI_PARSER_H
#define YOKAI_PARSER_H

#include "yokai/arena.h"
#include "yokai/ast.h"
#include "yokai/lexer.h"
#include "yokai/token.h"

typedef struct Parser {
  Lexer *lexer;
  Token current_token;
  Token peek_token;

  char **errors;
  size_t error_count;
  size_t error_cap;
} Parser;

/* reads the next token into the parser */
void p__next_token(Parser *p);

/* initializes the parser with current and peek token */
void parser_init(Parser *p, Lexer *l, Arena *arena);

/* check if the current token type is the same as provided */
bool p__current_token_is(Parser *p, TokenType ttype);

/* check if the peek token type is the same as provided */
bool p__peek_token_is(Parser *p, TokenType ttype);

/* moves ahead if the peek token matches the provided one */
bool p__expect_peek(Parser *p, Arena *arena, TokenType ttype);

/* adds a new error to the parser's errors array, handles alloc */
void parser_add_error(Parser *p, Arena *arena, const char *);

/* adds error to the parser by checking and matching the peek token with the exepcted token */
void p__peek_error(Parser *p, Arena *arena, TokenType ttype);

/* returns all the errors encountered by the parser */
char **p__errors(Parser *p);

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
