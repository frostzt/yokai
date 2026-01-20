/*
 * ast.h - Abstract Syntax Tree logic for Yokai
 *
 * Author: frostzt
 * Date: 2026-01-06
 */

#ifndef YOKAI_AST_H
#define YOKAI_AST_H

#include <stddef.h>

#include "yokai/arena.h"
#include "yokai/str.h"
#include "yokai/strbuf.h"
#include "yokai/token.h"

typedef enum {
  STMT_LET,
  STMT_RETURN,
  STMT_EXPR,
} StatementKind;

typedef enum {
  EXPR_IDENT,
  EXPR_INT,
  EXPR_FLOAT,
  EXPR_PREFIX,
  EXPR_INFIX,
} ExpressionKind;

typedef struct Expression {
  ExpressionKind kind;
  Token token;
  /* clang-format off */
  union {
    struct { StrView value; } expr_ident;
    struct { int64_t value; } expr_int_literal;
    struct { double value; } expr_float_literal;
    struct { StrView op; struct Expression* right; } expr_prefix;
    struct { struct Expression* left; StrView op; struct Expression* right; } expr_infix;
  } as;
  /* clang-format on */
} Expression;

typedef struct Statement {
  StatementKind kind;
  Token token;
  union {
    /* clang-format off */
    struct { Expression* name; Expression* value; } stmt_let;
    struct { Expression* return_value; } stmt_return;
    struct { Expression* expr; } stmt_expr;
    /* clang-format on */
  } as;
} Statement;

typedef struct Program {
  Statement **statements;
  size_t stmt_count;
  size_t stmt_capacity;
} Program;

/*----------------------------------------------------------------
 *  Core ast methods -- Statements
 *----------------------------------------------------------------*/

/* Allocates a new ExpressionStatement using the arena allocator */
Statement *ast_expr_stmt_new(Arena *arena, Token token, Expression *value);

/* Allocates a new LetStatement using the arena allocator */
Statement *ast_expr_let_new(Arena *arena, Token token, Expression *name, Expression *value);

/* Allocates a new LetStatement using the arena allocator */
Statement *ast_expr_return_new(Arena *arena, Token token, Expression *value);

/*----------------------------------------------------------------
 *  AST to String
 *----------------------------------------------------------------*/

/* builds a string representation of a program */
void ast__prog_to_string(const Program *prog, StrBuf *str_buf);

/* builds a string representation of a statement */
void ast__stmt_to_string(const Statement *stmt, StrBuf *str_buf);

/* builds a string representation of an expression */
void ast__expr_to_string(const Expression *expr, StrBuf *str_buf);

/*----------------------------------------------------------------
 *  Core ast methods
 *----------------------------------------------------------------*/

/* gets the literal for the provided token */
const char *stmt_token_literal(const Statement *s);

/* safely adds a new statement into the program ensuring capacity in the heap */
void program_add_statement(Program *prog, Arena *arena, Statement *stmt);

/* Allocates a new Program using the arena allocator with the provided capcity */
Program *ast_program_new(Arena *arena, size_t capacity);

/* Allocates a new Identifier using the arena allocator */
Expression *ast_expr_ident_new(Arena *arena, Token token, StrView value);

/* Allocates a new IntegerLiteral using the arena allocator */
Expression *ast_expr_int_new(Arena *arena, Token token, int64_t value);

/* Allocates a new FloatLiteral using the arena allocator */
Expression *ast_expr_float_new(Arena *arena, Token token, double value);

/* Allocates a new Infix expression using the arena allocator */
Expression *ast_expr_infix_new(Arena *arena, Token operator, Expression * left_expr,
                               Expression *right_expr);

/* Allocates a new Prefix expression using the arena allocator */
Expression *ast_expr_prefix_new(Arena *arena, Token operator, Expression * expr);

#endif // YOKAI_AST_H
