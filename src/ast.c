/*
 * ast.c - Abstract Syntax Tree logic for Yokai
 *
 * Author: frostzt
 * Date: 2026-01-06
 */

#include <stdalign.h>
#include <stdio.h>
#include <string.h>

#include "yokai/arena.h"
#include "yokai/ast.h"

const char *stmt_token_literal(const Statement *s) {
  switch (s->kind) {
  case STMT_LET: {
    const LetStatement *ls = (const LetStatement *)s;
    return ls->token.literal.data;
  }
  case STMT_RETURN: {
    const ReturnStatement *rs = (const ReturnStatement *)s;
    return rs->token.literal.data;
  }
  case STMT_EXPR: {
    const ExpressionStatement *es = (const ExpressionStatement *)s;
    return es->token.literal.data;
  }
  }

  return NULL;
}

void ast__prog_to_string(const Program *prog, StrBuf *str_buf) {
  for (size_t i = 0; i < prog->stmt_count; i++) {
    ast__stmt_to_string(prog->statements[i], str_buf);
  }
}

void ast__stmt_to_string(const Statement *stmt, StrBuf *str_buf) {
  switch (stmt->kind) {
  case STMT_LET: {
    const LetStatement *let_stmt = (const LetStatement *)stmt;
    sb__append_strview(str_buf, let_stmt->token.literal); // let
    sb__append_cstr(str_buf, " ");
    sb__append_strview(str_buf, let_stmt->name->value);
    sb__append_cstr(str_buf, " = ");
    if (let_stmt->value) { ast__expr_to_string(let_stmt->value, str_buf); }
    sb__append_cstr(str_buf, ";");
    break;
  };

  case STMT_RETURN: {
    const ReturnStatement *rtn_stmt = (const ReturnStatement *)stmt;
    sb__append_strview(str_buf, rtn_stmt->token.literal); // return
    sb__append_cstr(str_buf, " ");
    if (rtn_stmt->return_value) { ast__expr_to_string(rtn_stmt->return_value, str_buf); }
    sb__append_cstr(str_buf, ";");
    break;
  };

  case STMT_EXPR: {
    const ExpressionStatement *expr_stmt = (const ExpressionStatement *)stmt;
    if (expr_stmt->expression) { ast__expr_to_string(expr_stmt->expression, str_buf); }
    sb__append_cstr(str_buf, ";");
    break;
  };
  }
}

void ast__expr_to_string(const Expression *expr, StrBuf *str_buf) {
  switch (expr->kind) {
  case EXPR_IDENT: {
    const Identifier *id = (const Identifier *)expr;
    sb__append_strview(str_buf, id->value);
    break;
  }
  case EXPR_INT: {
    const IntegerLiteral *il = (const IntegerLiteral *)expr;
    char buf[32];
    int n = snprintf(buf, sizeof(buf), "%lld", (long long)il->value);
    sb__append(str_buf, buf, n);
    break;
  }
  case EXPR_FLOAT: {
    const FloatLiteral *fl = (const FloatLiteral *)expr;
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "%g", fl->value);
    sb__append(str_buf, buf, n);
    break;
  }
  case EXPR_PREFIX:
  case EXPR_INFIX:
    break;
  }
}

void ast__ident_to_string(const Identifier *ident, StrBuf *str_buf) {
  sb__append_strview(str_buf, ident->value);
}

/* checks and ensures program has capacity to store more statements allocates new if not */
static void program_ensure_capacity(Program *program, Arena *arena) {
  if (program->stmt_count < program->stmt_capacity) {
    /* return as we still have space */
    return;
  }

  size_t new_cap = (program->stmt_capacity == 0) ? 4 : program->stmt_capacity * 2;
  Statement **new_arr = arena_alloc(arena, sizeof(Statement *) * new_cap, alignof(Statement *));
  if (program->statements && program->stmt_count > 0) {
    memcpy(new_arr, program->statements, sizeof(Statement *) * program->stmt_count);
  }

  program->statements = new_arr;
  program->stmt_capacity = new_cap;
}

void program_add_statement(Program *prog, Arena *arena, Statement *stmt) {
  program_ensure_capacity(prog, arena);
  prog->statements[prog->stmt_count++] = stmt;
}

Program *ast_program_new(Arena *arena, size_t capacity) {
  Program *prog = arena_alloc(arena, sizeof(Program), alignof(Program));
  prog->stmt_count = 0;
  prog->stmt_capacity = capacity;
  prog->statements = arena_alloc(arena, sizeof(Statement *) * capacity, alignof(Statement *));
  memset(prog->statements, 0, sizeof(Statement *) * capacity);
  return prog;
}

LetStatement *ast_let_new(Arena *arena, Token token, Identifier *name, Expression *value) {
  LetStatement *let_stmt = arena_alloc(arena, sizeof(LetStatement), alignof(LetStatement));
  let_stmt->base.kind = STMT_LET;
  let_stmt->token = token;
  let_stmt->name = name;
  let_stmt->value = value;
  return let_stmt;
}

ReturnStatement *ast_return_new(Arena *arena, Token token, Expression *value) {
  ReturnStatement *rtn_stmt = arena_alloc(arena, sizeof(ReturnStatement), alignof(ReturnStatement));
  rtn_stmt->base.kind = STMT_RETURN;
  rtn_stmt->token = token;
  rtn_stmt->return_value = value;
  return rtn_stmt;
}

ExpressionStatement *ast_expr_stmt_new(Arena *arena, Token token, Expression *value) {
  ExpressionStatement *expr_stmt =
      arena_alloc(arena, sizeof(ExpressionStatement), alignof(ExpressionStatement));
  expr_stmt->base.kind = STMT_EXPR;
  expr_stmt->token = token;
  expr_stmt->expression = value;
  return expr_stmt;
}

Identifier *ast_ident_new(Arena *arena, Token token, StrView value) {
  Identifier *ident = arena_alloc(arena, sizeof(Identifier), alignof(Identifier));
  ident->token = token;
  ident->base.kind = EXPR_IDENT;
  ident->value = value;
  return ident;
}

IntegerLiteral *ast_int_new(Arena *arena, Token token, int64_t value) {
  IntegerLiteral *intLit = arena_alloc(arena, sizeof(IntegerLiteral), alignof(IntegerLiteral));
  intLit->token = token;
  intLit->base.kind = EXPR_INT;
  intLit->value = value;
  return intLit;
}

FloatLiteral *ast_float_new(Arena *arena, Token token, double value) {
  FloatLiteral *floatLit = arena_alloc(arena, sizeof(FloatLiteral), alignof(FloatLiteral));
  floatLit->token = token;
  floatLit->base.kind = EXPR_FLOAT;
  floatLit->value = value;
  return floatLit;
}
