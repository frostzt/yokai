/*
 * parser.c - Recursive descent parser for Yokai
 *
 * Author: frostzt
 * Date: 2026-01-05
 */

#include <stdalign.h>
#include <stdio.h>
#include <string.h>

#include "yokai/arena.h"
#include "yokai/ast.h"
#include "yokai/lexer.h"
#include "yokai/parser.h"
#include "yokai/token.h"

void p_next_token(Parser *p) {
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
  default:
    return NULL;
  }
}

LetStatement *parse_let_statement(Parser *p, Arena *arena) {
  if (!expect_peek(p, TOK_IDENT)) { return NULL; }

  /* create let token */
  Token let_tok;
  let_tok.type = TOK_LET;
  let_tok.literal = sv_from_cstr("let");

  /* create a new token for identifier */
  Token ident_token;
  ident_token.type = TOK_IDENT;
  ident_token.literal = p->current_token.literal;
  /* create a new identifier from the arena allocator */
  Identifier *ident_name = ast_ident_new(arena, ident_token, ident_token.literal);

  /* next token should always be an assignment post that we can do pratt parsing */
  if (!expect_peek(p, TOK_ASSIGN)) { return NULL; }

  // TODO: Expressions
  if (!expect_peek(p, TOK_SEMICOLON)) { p_next_token(p); }

  LetStatement *let_stmt = ast_let_new(arena, let_tok, ident_name, NULL);
  return let_stmt;
}

Program *parse_program(Parser *p, Arena *arena) {
  Program *program = ast_program_new(arena, 8);
  /* loop until we encounter an EOF token */
  while (p->current_token.type != TOK_EOF) {
    Statement *stmt = (Statement *)parse_statement(p, arena);
    if (stmt != NULL) { program->statements[program->stmt_count++] = stmt; }
    p_next_token(p);
  }
  return program;
}

void peek_error(Parser *p, Arena *arena, TokenType ttype) {
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

bool expect_peek(Parser *p, TokenType ttype) {
  if (peek_token_is(p, ttype)) {
    p_next_token(p);
    return true;
  } else {
    return false;
  }
}

bool peek_token_is(Parser *p, TokenType ttype) {
  return p->peek_token.type == ttype;
}

bool current_token_is(Parser *p, TokenType ttype) {
  return p->current_token.type == ttype;
}
