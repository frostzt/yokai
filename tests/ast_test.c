#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
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
  Expression *ident = ast_expr_ident_new(&ar, ident_tok, ident_tok.literal);
  Expression *intLiteral = ast_expr_int_new(&ar, int_tok, 5);

  Statement *letStmt = ast_expr_let_new(&ar, let_tok, ident, (Expression *)intLiteral);

  prog->statements[0] = (Statement *)letStmt;
  prog->stmt_count = 1;

  // assertions
  ASSERT_EQ(prog->stmt_count, 1, "invalid stmt count");
  Statement *s = prog->statements[0];
  ASSERT_EQ(s->kind, STMT_LET, "invalid statement");

  ASSERT(sv_eq_cstr(s->as.stmt_let.name->as.expr_ident.value, "my_var"),
         "let statement ident mismatch");
  ASSERT_EQ(s->as.stmt_let.value->as.expr_int_literal.value, 5, "integral literal mismatch");

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
  Expression *ident = ast_expr_ident_new(&arena, ident_tok, ident_tok.literal);
  Expression *another_ident =
      ast_expr_ident_new(&arena, another_ident_tok, another_ident_tok.literal);
  Statement *let_stmt = ast_expr_let_new(&arena, let_tok, ident, another_ident);

  prog->statements[0] = (Statement *)let_stmt;
  prog->stmt_count = 1;

  /* create a new string buffer */
  StrBuf str_buf;
  str_buf.arena = &arena;
  sb__init(&str_buf, 32);

  /* convert the program into a string */
  ast__prog_to_string(prog, &str_buf);

  StrView prog_sv = sb__view(&str_buf);
  sv_eq_cstr(prog_sv, "let my_var = another_var;");
}
