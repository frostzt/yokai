#include <string.h>

#include "test.h"
#include "yokai/arena.h"
#include "yokai/ast.h"
#include "yokai/str.h"

TEST(ast_simple_stmt) {
  Arena ar = arena_create(128);

  // let token
  Token let_tok;
  let_tok.type = TOK_LET;
  let_tok.literal = sv_from_cstr("let");

  // identifier token
  Token ident_tok;
  ident_tok.type = TOK_IDENT;
  ident_tok.literal = sv_from_cstr("my_var");

  // int token
  Token int_tok;
  int_tok.type = TOK_INT;
  int_tok.literal = sv_from_cstr("5");

  Program *prog = ast_program_new(&ar, 8);
  Identifier *ident = ast_ident_new(&ar, ident_tok, ident_tok.literal);
  IntegerLiteral *intLiteral = ast_int_new(&ar, int_tok, 5);

  LetStatement *letStmt = ast_let_new(&ar, let_tok, ident, (Expression *)intLiteral);

  prog->statements[0] = (Statement *)letStmt;
  prog->stmt_count = 1;

  // assertions
  ASSERT_EQ(prog->stmt_count, 1);
  Statement *s = prog->statements[0];
  ASSERT_EQ(s->kind, STMT_LET);

  LetStatement *ls = (LetStatement *)s;
  ASSERT(sv_eq_cstr(ls->name->value, "my_var"));
  ASSERT_EQ(((IntegerLiteral *)ls->value)->value, 5);

  arena_destroy(&ar);
}
