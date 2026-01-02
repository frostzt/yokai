#include <stdalign.h>
#include <string.h>

#include "yokai/arena.h"
#include "yokai/ast.h"

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
