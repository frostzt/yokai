/*
 * ast.c - Abstract Syntax Tree logic for Yokai
 *
 * Author: frostzt
 * Date: 2026-01-06
 */
#include "yokai/ast.h"
#include "yokai/arena.h"

const char *stmt_token_literal(const Statement *s) {
  switch (s->kind) {
  case STMT_RETURN:
  case STMT_EXPR:
  case STMT_LET:
  case STMT_BLOCK:
    return s->token.literal.data;
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
    sb__append_strview(str_buf, stmt->token.literal); // let
    sb__append_cstr(str_buf, " ");
    sb__append_strview(str_buf, stmt->as.stmt_let.name->as.expr_ident.value);
    sb__append_cstr(str_buf, " = ");
    if (stmt->as.stmt_let.value) { ast__expr_to_string(stmt->as.stmt_let.value, str_buf); }
    sb__append_cstr(str_buf, ";");
    break;
  };
  case STMT_RETURN: {
    sb__append_strview(str_buf, stmt->token.literal); // return
    sb__append_cstr(str_buf, " ");
    if (stmt->as.stmt_return.return_value) {
      ast__expr_to_string(stmt->as.stmt_return.return_value, str_buf);
    }
    sb__append_cstr(str_buf, ";");
    break;
  };
  case STMT_BLOCK: {
    for (size_t i = 0; i < stmt->as.stmt_block.statement_count; i++) {
      Statement *s = stmt->as.stmt_block.stmts[i];
      ast__stmt_to_string(s, str_buf);
    }
    break;
  }
  case STMT_EXPR: {
    if (stmt->as.stmt_expr.expr) { ast__expr_to_string(stmt->as.stmt_expr.expr, str_buf); }
    break;
  };
  }
}

void ast__expr_to_string(const Expression *expr, StrBuf *str_buf) {
  switch (expr->kind) {
  case EXPR_IDENT: {
    sb__append_strview(str_buf, expr->as.expr_ident.value);
    break;
  }
  case EXPR_INT: {
    char buf[32];
    int n = snprintf(buf, sizeof(buf), "%lld", expr->as.expr_int_literal.value);
    sb__append(str_buf, buf, n);
    break;
  }
  case EXPR_FLOAT: {
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "%g", expr->as.expr_float_literal.value);
    sb__append(str_buf, buf, n);
    break;
  }
  case EXPR_PREFIX: {
    sb__append_cstr(str_buf, "(");
    sb__append_strview(str_buf, expr->as.expr_prefix.op);
    ast__expr_to_string(expr->as.expr_prefix.right, str_buf);
    sb__append_cstr(str_buf, ")");
    break;
  }
  case EXPR_INFIX: {
    sb__append_cstr(str_buf, "(");
    ast__expr_to_string(expr->as.expr_infix.left, str_buf);
    sb__append_cstr(str_buf, " ");
    sb__append_strview(str_buf, expr->as.expr_infix.op);
    sb__append_cstr(str_buf, " ");
    ast__expr_to_string(expr->as.expr_infix.right, str_buf);
    sb__append_cstr(str_buf, ")");
    break;
  }
  case EXPR_IF: {
    sb__append_cstr(str_buf, "if");
    ast__expr_to_string(expr->as.expr_if.condition, str_buf);
    sb__append_cstr(str_buf, " ");
    ast__stmt_to_string(expr->as.expr_if.consequence, str_buf);
    if (expr->as.expr_if.alternative != NULL) {
      sb__append_cstr(str_buf, "else");
      ast__stmt_to_string(expr->as.expr_if.alternative, str_buf);
    }
    break;
  }
  case EXPR_BOOLEAN: {
    sb__append_cstr(str_buf, expr->as.expr_bool_literal.value ? "true" : "false");
    break;
  }
  }
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

Statement *ast_expr_let_new(Arena *arena, Token token, Expression *name, Expression *value) {
  Statement *let_stmt = arena_alloc(arena, sizeof(Statement), alignof(Statement));
  let_stmt->kind = STMT_LET;
  let_stmt->token = token;
  let_stmt->as.stmt_let.name = name;
  let_stmt->as.stmt_let.value = value;
  return let_stmt;
}

Statement *ast_expr_return_new(Arena *arena, Token token, Expression *value) {
  Statement *rtn_stmt = arena_alloc(arena, sizeof(Statement), alignof(Statement));
  rtn_stmt->kind = STMT_RETURN;
  rtn_stmt->token = token;
  rtn_stmt->as.stmt_return.return_value = value;
  return rtn_stmt;
}

Statement *ast_expr_stmt_new(Arena *arena, Token token, Expression *value) {
  Statement *expr_stmt = arena_alloc(arena, sizeof(Statement), alignof(Statement));
  expr_stmt->kind = STMT_EXPR;
  expr_stmt->token = token;
  expr_stmt->as.stmt_expr.expr = value;
  return expr_stmt;
}

Statement *ast_expr_block_new(Arena *arena, Token token, size_t initial_capacity) {
  Statement *expr_stmt = arena_alloc(arena, sizeof(Statement), alignof(Statement));
  expr_stmt->kind = STMT_BLOCK;
  expr_stmt->token = token;
  expr_stmt->as.stmt_block.statement_count = 0;
  expr_stmt->as.stmt_block.statement_capacity = initial_capacity;
  expr_stmt->as.stmt_block.stmts =
      arena_alloc(arena, sizeof(Statement *) * initial_capacity, alignof(Statement *));

  memset(expr_stmt->as.stmt_block.stmts, 0, sizeof(Statement *) * initial_capacity);
  return expr_stmt;
}

Expression *ast_expr_prefix_new(Arena *arena, Token operator, Expression *expr) {
  Expression *prefix_expr = arena_alloc(arena, sizeof(Expression), alignof(Expression));
  prefix_expr->token = operator; /* operator */
  prefix_expr->kind = EXPR_PREFIX;
  prefix_expr->as.expr_prefix.right = expr;
  prefix_expr->as.expr_prefix.op = operator.literal;
  return prefix_expr;
}

Expression *ast_expr_infix_new(Arena *arena, Token operator, Expression *left_expr,
                               Expression *right_expr) {
  Expression *infix_expr = arena_alloc(arena, sizeof(Expression), alignof(Expression));
  infix_expr->token = operator; /* operator */
  infix_expr->kind = EXPR_INFIX;
  infix_expr->as.expr_infix.left = left_expr;
  infix_expr->as.expr_infix.op = operator.literal;
  infix_expr->as.expr_infix.right = right_expr;
  return infix_expr;
}

Expression *ast_expr_ident_new(Arena *arena, Token token, StrView value) {
  Expression *ident = arena_alloc(arena, sizeof(Expression), alignof(Expression));
  ident->token = token;
  ident->kind = EXPR_IDENT;
  ident->as.expr_ident.value = value;
  return ident;
}

Expression *ast_expr_int_new(Arena *arena, Token token, int64_t value) {
  Expression *int_lit = arena_alloc(arena, sizeof(Expression), alignof(Expression));
  int_lit->token = token;
  int_lit->kind = EXPR_INT;
  int_lit->as.expr_int_literal.value = value;
  return int_lit;
}

Expression *ast_expr_bool_new(Arena *arena, Token token, bool value) {
  Expression *bool_lit = arena_alloc(arena, sizeof(Expression), alignof(Expression));
  bool_lit->token = token;
  bool_lit->kind = EXPR_BOOLEAN;
  bool_lit->as.expr_bool_literal.value = value;
  return bool_lit;
}

Expression *ast_expr_float_new(Arena *arena, Token token, double value) {
  Expression *float_lit = arena_alloc(arena, sizeof(Expression), alignof(Expression));
  float_lit->token = token;
  float_lit->kind = EXPR_FLOAT;
  float_lit->as.expr_float_literal.value = value;
  return float_lit;
}

Expression *ast_expr_if_new(Arena *arena, Token token, Expression *condition,
                            Statement *consequence, Statement *alternative) {
  Expression *if_expr = arena_alloc(arena, sizeof(Expression), alignof(Expression));
  if_expr->token = token;
  if_expr->kind = EXPR_IF;
  if_expr->as.expr_if.condition = condition;
  if_expr->as.expr_if.consequence = consequence;
  if_expr->as.expr_if.alternative = alternative;
  return if_expr;
}
