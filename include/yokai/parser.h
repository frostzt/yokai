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

struct Parser;

/* Function that parses a prefix expression
 *      -21
 *      ^ -- prefix, "-21" is the expression */
typedef Expression *(*PrefixParseFn)(struct Parser *);

/* Function that parses a infix expression
 *      2 + 2
 *        ^ -- infix, "2 + 2" is the expression */
typedef Expression *(*InfixParseFn)(struct Parser *, Expression *left);

/* Precedence of each operator in the Yokai Language */
typedef enum {
  PREC_LOWEST,
  PREC_EQUALS,      // == !=
  PREC_LESSGREATER, // < >
  PREC_SUM,         // + -
  PREC_PRODUCT,     // * /
  PREC_PREFIX,      // -X !X
  PREC_CALL,        // fn(x)
} Precedence;

/* Core parser struct that parses the entire program */
typedef struct Parser {
  Lexer *lexer;
  Token current_token;
  Token peek_token;

  char **errors;
  size_t error_count;
  size_t error_cap;

  PrefixParseFn prefix_fns[TOK__COUNT];
  InfixParseFn infix_fns[TOK__COUNT];
  Precedence prec[TOK__COUNT];
} Parser;

/*----------------------------------------------------------------
 *  Token parsing helpers
 *----------------------------------------------------------------*/

/* reads the next token into the parser */
void p__next_token(Parser *p);

/* check if the current token type is the same as provided */
bool p__current_token_is(Parser *p, TokenType ttype);

/* check if the peek token type is the same as provided */
bool p__peek_token_is(Parser *p, TokenType ttype);

/* moves ahead if the peek token matches the provided one */
bool p__expect_peek(Parser *p, Arena *arena, TokenType ttype);

/*----------------------------------------------------------------
 *  Helpers
 *----------------------------------------------------------------*/

/* adds a new error to the parser's errors array, handles alloc */
void parser_add_error(Parser *p, Arena *arena, const char *);

/* adds error to the parser by checking and matching the peek token with the exepcted token */
void p__peek_error(Parser *p, Arena *arena, TokenType ttype);

/* returns all the errors encountered by the parser */
char **p__errors(Parser *p);

/* initializes the parser with current and peek token */
void parser_init(Parser *p, Lexer *l, Arena *arena);

/* registers a prefix function -- `operator operand` */
void register_prefix(Parser *p, TokenType ttype, PrefixParseFn fn);

/* registers an infix function -- `operand operator operand` */
void register_infix(Parser *p, TokenType ttype, InfixParseFn fn);

/*----------------------------------------------------------------
 *  Core parser methods
 *----------------------------------------------------------------*/

/* parses a "let" statement */
LetStatement *parse_let_statement(Parser *p, Arena *arena);

/* parses a "return" statement */
ReturnStatement *parse_return_statement(Parser *p, Arena *arena);

/* parses a statement and returns */
void *parse_statement(Parser *p, Arena *arena);

/* traverses the entire program */
Program *parse_program(Parser *p, Arena *arena);

/*----------------------------------------------------------------
 *  Core expression parsing method
 *----------------------------------------------------------------*/

/* parses an expression statement */
ExpressionStatement *parse_expression_statement(Parser *p, Arena *arena);

/* parses an expression */
Expression *parse_expression(Parser *p);

#endif // YOKAI_PARSER_H
