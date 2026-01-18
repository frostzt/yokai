#include <stdint.h>
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
  ASSERT_EQ(prog->stmt_count, 1, "invalid stmt count");
  Statement *s = prog->statements[0];
  ASSERT_EQ(s->kind, STMT_LET, "invalid statement");

  LetStatement *ls = (LetStatement *)s;
  ASSERT(sv_eq_cstr(ls->name->value, "my_var"), "let statement ident mismatch");
  ASSERT_EQ(((IntegerLiteral *)ls->value)->value, 5, "integral literal mismatch");

  arena_destroy(&ar);
}

TEST(ast_to_string) {
  Arena arena = arena_create(128);

  // let token
  Token let_tok;
  let_tok.type = TOK_LET;
  let_tok.literal = sv_from_cstr("let");

  // identifier token
  Token ident_tok;
  ident_tok.type = TOK_IDENT;
  ident_tok.literal = sv_from_cstr("my_var");

  // another identifier token
  Token another_ident_tok;
  another_ident_tok.type = TOK_IDENT;
  another_ident_tok.literal = sv_from_cstr("another_var");

  Program *prog = ast_program_new(&arena, 8);
  Identifier *ident = ast_ident_new(&arena, ident_tok, ident_tok.literal);
  Identifier *another_ident = ast_ident_new(&arena, another_ident_tok, another_ident_tok.literal);
  LetStatement *let_stmt = ast_let_new(&arena, let_tok, ident, (Expression *)another_ident);

  prog->statements[0] = (Statement *)let_stmt;

  /* create a new string buffer */
  StrBuf str_buf;
  str_buf.arena = &arena;
  sb__init(&str_buf, 32);

  /* convert the program into a string */
  ast__prog_to_string(prog, &str_buf);
}
