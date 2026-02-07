/*
 * parser.c - Recursive descent parser for Yokai
 *
 * Author: frostzt
 * Date: 2026-01-05
 */

#include <errno.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yokai/arena.h"
#include "yokai/ast.h"
#include "yokai/lexer.h"
#include "yokai/parser.h"
#include "yokai/str.h"
#include "yokai/token.h"
#include "yokai/util.h"

void p__next_token(Parser *p) {
  p->current_token = p->peek_token;
  p->peek_token = next_token(p->lexer);
}

void parser_init(Parser *p, Lexer *l, Arena *arena) {
  p->lexer = l;
  p->errors = arena_alloc(arena, sizeof(char *) * 8, alignof(char *));
  p->error_cap = 8;
  p->error_count = 0;

  p->current_token = next_token(p->lexer);
  p->peek_token = next_token(p->lexer);

  /* init precedence */
  p->prec[TOK_EQ] = p->prec[TOK_NOT_EQ] = PREC_EQUALS;
  p->prec[TOK_LT] = p->prec[TOK_GT] = PREC_LESSGREATER;
  p->prec[TOK_PLUS] = p->prec[TOK_MINUS] = PREC_SUM;
  p->prec[TOK_ASTERISK] = p->prec[TOK_SLASH] = PREC_PRODUCT;
  p->prec[TOK_LPAREN] = PREC_CALL;

  /* init prefix parsers */
  register_prefix(p, TOK_IDENT, parse_identifier);
  register_prefix(p, TOK_INT, parse_integer_literal);
  register_prefix(p, TOK_FLOAT, parse_float_literal);
  register_prefix(p, TOK_BANG, parse_prefix_expression);
  register_prefix(p, TOK_MINUS, parse_prefix_expression);
  register_prefix(p, TOK_TRUE, parse_boolean);
  register_prefix(p, TOK_FALSE, parse_boolean);
  register_prefix(p, TOK_LPAREN, parse_grouped_expression);

  /* init infix parsers */
  register_infix(p, TOK_PLUS, parse_infix_expression);
  register_infix(p, TOK_MINUS, parse_infix_expression);
  register_infix(p, TOK_SLASH, parse_infix_expression);
  register_infix(p, TOK_ASTERISK, parse_infix_expression);
  register_infix(p, TOK_EQ, parse_infix_expression);
  register_infix(p, TOK_NOT_EQ, parse_infix_expression);
  register_infix(p, TOK_LT, parse_infix_expression);
  register_infix(p, TOK_GT, parse_infix_expression);
}

Precedence token_type_precedence(TokenType ttype) {
  switch (ttype) {
  case TOK_EQ:
  case TOK_NOT_EQ:
    return PREC_EQUALS;

  case TOK_LT:
  case TOK_GT:
    return PREC_LESSGREATER;

  case TOK_PLUS:
  case TOK_MINUS:
    return PREC_SUM;

  case TOK_SLASH:
  case TOK_ASTERISK:
    return PREC_PRODUCT;

  default:
    return PREC_LOWEST;
  }
}

Precedence p__peek_precedence(Parser *p) {
  return token_type_precedence(p->peek_token.type);
}

Precedence p__current_precedence(Parser *p) {
  return token_type_precedence(p->current_token.type);
}

Expression *parse_float_literal(Parser *p, Arena *arena) {
  double value;
  if (!safe_parse_double_sv(&p->current_token.literal, &value)) {
    StrBuf str_buf;
    str_buf.arena = arena;
    sb__init(&str_buf, 32);

    sb__append_cstr(&str_buf, "could not parse '");
    sb__append_cstr(&str_buf, p->current_token.literal.data);
    sb__append_cstr(&str_buf, "' as float");

    parser_add_error(p, arena, sb__cstr(&str_buf));
    return NULL;
  }

  /* use the parsed value and token to create a new float literal */
  Expression *float_literal = ast_expr_float_new(arena, p->current_token, value);
  return float_literal;
}

Expression *parse_integer_literal(Parser *p, Arena *arena) {
  int64_t value;
  if (!safe_parse_int64_sv(&p->current_token.literal, &value)) {
    StrBuf str_buf;
    str_buf.arena = arena;
    sb__init(&str_buf, 32);

    sb__append_cstr(&str_buf, "could not parse '");
    sb__append_cstr(&str_buf, p->current_token.literal.data);
    sb__append_cstr(&str_buf, "' as integer");

    parser_add_error(p, arena, sb__cstr(&str_buf));
    return NULL;
  }

  /* use the parsed value and token to create a new integer literal */
  Expression *int_literal = ast_expr_int_new(arena, p->current_token, value);
  return int_literal;
}

Expression *parse_boolean(Parser *p, Arena *arena) {
  return ast_expr_bool_new(arena, p->current_token, p__current_token_is(p, TOK_TRUE));
}

Expression *parse_identifier(Parser *p, Arena *arena) {
  return ast_expr_ident_new(arena, p->current_token, p->current_token.literal);
}

void parser_add_error(Parser *p, Arena *arena, const char *msg) {
  if (p->error_count == p->error_cap) {
    size_t new_cap = p->error_cap * 2;
    char **new_arr = arena_alloc(arena, sizeof(char *) * new_cap, alignof(char *));

    memcpy(new_arr, p->errors, sizeof(char *) * p->error_count);
    p->errors = new_arr;
    p->error_cap = new_cap;
  }

  p->errors[p->error_count++] = (char *)msg;
}

void *parse_statement(Parser *p, Arena *arena) {
  switch (p->current_token.type) {
  case TOK_LET:
    return parse_let_statement(p, arena);
  case TOK_RETURN:
    return parse_return_statement(p, arena);
  default:
    return parse_expression_statement(p, arena);
  }
}

void register_prefix(Parser *p, TokenType ttype, PrefixParseFn fn) {
  p->prefix_fns[ttype] = fn;
}

void register_infix(Parser *p, TokenType ttype, InfixParseFn fn) {
  p->infix_fns[ttype] = fn;
}

Statement *parse_expression_statement(Parser *p, Arena *arena) {
  Expression *expr = parse_expression(p, arena, PREC_LOWEST);
  if (expr == NULL) { return NULL; }

  if (p__peek_token_is(p, TOK_SEMICOLON)) { p__next_token(p); }

  Statement *expr_stmt = ast_expr_stmt_new(arena, p->current_token, expr);
  return expr_stmt;
}

Expression *parse_expression(Parser *p, Arena *arena, Precedence precedence) {
  PrefixParseFn prefix = p->prefix_fns[p->current_token.type];
  if (prefix == NULL) {
    no_prefix_parse_fn_error(p, arena, p->current_token.type);
    return NULL;
  }
  Expression *left_expression = prefix(p, arena);

  /* try and find an infix parsing function for the next token */
  while (!p__peek_token_is(p, TOK_SEMICOLON) && precedence < p__peek_precedence(p)) {
    InfixParseFn infix = p->infix_fns[p->peek_token.type];
    if (infix == NULL) { return left_expression; }

    p__next_token(p);
    /* this keeps resolving the left, the infix function will call for the right */
    left_expression = infix(p, arena, left_expression);
  }

  return left_expression;
}

Expression *parse_grouped_expression(Parser *p, Arena *arena) {
  p__next_token(p);
  Expression *expr = parse_expression(p, arena, PREC_LOWEST);
  if (!p__expect_peek(p, arena, TOK_RPAREN)) { return NULL; }
  return expr;
}

Expression *parse_prefix_expression(Parser *p, Arena *arena) {
  Token current_token = p->current_token;
  /* advance the token to get the next value */
  p__next_token(p);
  Expression *right_expr = parse_expression(p, arena, PREC_PREFIX);
  /* note its `current_token` copied above NOT p->current_token */
  Expression *expr = ast_expr_prefix_new(arena, current_token, right_expr);
  return expr;
}

Expression *parse_infix_expression(Parser *p, Arena *arena, Expression *left_expr) {
  Token current_token = p->current_token;
  Precedence precedence = p__current_precedence(p);
  /* advance the token to get the next value */
  p__next_token(p);
  Expression *right_expr = parse_expression(p, arena, precedence);
  Expression *expr = ast_expr_infix_new(arena, current_token, left_expr, right_expr);
  return expr;
}

void no_prefix_parse_fn_error(Parser *p, Arena *arena, TokenType tt) {
  StrBuf str_buf;
  sb__init(&str_buf, 32);

  sb__append_cstr(&str_buf, "no prefix function for '");
  sb__append_cstr(&str_buf, token_type_name(tt));
  sb__append_cstr(&str_buf, "' found");

  parser_add_error(p, arena, sb__cstr(&str_buf));
}

Statement *parse_return_statement(Parser *p, Arena *arena) {
  if (!p__current_token_is(p, TOK_RETURN)) { return NULL; }

  /* create return token */
  Token return_tok;
  return_tok.type = TOK_RETURN;
  return_tok.literal = sv_from_cstr("return");

  // TODO: Expressions, skipping
  while (!p__current_token_is(p, TOK_SEMICOLON)) {
    p__next_token(p);
  }

  Statement *rtn_stmt = ast_expr_return_new(arena, return_tok, NULL);
  return rtn_stmt;
}

Statement *parse_let_statement(Parser *p, Arena *arena) {
  if (!p__current_token_is(p, TOK_LET)) { return NULL; }
  if (!p__expect_peek(p, arena, TOK_IDENT)) { return NULL; }

  /* create let token */
  Token let_tok;
  let_tok.type = TOK_LET;
  let_tok.literal = sv_from_cstr("let");

  /* create a new token for identifier */
  Token ident_token;
  ident_token.type = TOK_IDENT;
  ident_token.literal = p->current_token.literal;
  /* create a new identifier from the arena allocator */
  Expression *ident_name = ast_expr_ident_new(arena, ident_token, ident_token.literal);

  /* next token should always be an assignment post that we can do pratt parsing */
  if (!p__expect_peek(p, arena, TOK_ASSIGN)) { return NULL; }

  // TODO: Expressions, right now skipping
  while (!p__current_token_is(p, TOK_SEMICOLON)) {
    p__next_token(p);
  }

  Statement *let_stmt = ast_expr_let_new(arena, let_tok, ident_name, NULL);
  return let_stmt;
}

Program *parse_program(Parser *p, Arena *arena) {
  Program *program = ast_program_new(arena, 8);
  /* loop until we encounter an EOF token */
  while (p->current_token.type != TOK_EOF) {
    Statement *stmt = (Statement *)parse_statement(p, arena);
    if (stmt != NULL) { program_add_statement(program, arena, stmt); }
    p__next_token(p);
  }
  return program;
}

char **p__errors(Parser *p) {
  return p->errors;
}

void p__peek_error(Parser *p, Arena *arena, TokenType ttype) {
  const char *exp = token_type_name(ttype);
  const char *got = token_type_name(p->peek_token.type);

  /* format on buffer in stack */
  char buf[128];
  int n = snprintf(buf, sizeof(buf), "expected next token to be %s, got %s instead", exp, got);

  /* copy t arena */
  char *stored = arena_alloc(arena, n + 1, 1);
  memcpy(stored, buf, n + 1);

  parser_add_error(p, arena, stored);
}

bool p__expect_peek(Parser *p, Arena *arena, TokenType ttype) {
  if (p__peek_token_is(p, ttype)) {
    p__next_token(p);
    return true;
  } else {
    p__peek_error(p, arena, ttype);
    return false;
  }
}

bool p__peek_token_is(Parser *p, TokenType ttype) {
  return p->peek_token.type == ttype;
}

bool p__current_token_is(Parser *p, TokenType ttype) {
  return p->current_token.type == ttype;
}
