#include <stdalign.h>
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
  LetStatement *letStmt = arena_alloc(arena, sizeof(LetStatement), alignof(LetStatement));
  letStmt->base.kind = STMT_LET;
  letStmt->token = token;
  letStmt->name = name;
  letStmt->value = value;
  return letStmt;
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
