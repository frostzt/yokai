#ifndef YOKAI_AST_H
#define YOKAI_AST_H

#include <stddef.h>

#include "yokai/arena.h"
#include "yokai/str.h"
#include "yokai/token.h"

typedef enum {
  STMT_LET,
  STMT_RETURN,
  STMT_EXPR,
} StatementKind;

typedef enum {
  EXPR_IDENT,
  EXPR_INT,
  EXPR_PREFIX,
  EXPR_INFIX,
} ExpressionKind;

typedef struct Statement {
  StatementKind kind;
} Statement;

typedef struct Expression {
  ExpressionKind kind;
} Expression;

typedef struct Program {
  Statement **statements;
  size_t stmt_count;
  size_t stmt_capacity;
} Program;

typedef struct Identifier {
  Expression base; // EXPR_IDENT
  Token token;
  StrView value;
} Identifier;

typedef struct LetStatement {
  Statement base; // STMT_LET
  Token token;    // 'let'
  Identifier *name;
  Expression *value;
} LetStatement;

typedef struct ReturnStatement {
  Statement base; // STMT_RETURN
  Token token;    // 'return'
  Expression *return_value;
} ReturnStatement;

typedef struct ExpressionStatement {
  Statement base; // STMT_EXPR
  Token token;
  Expression *expression;
} ExpressionStatement;

// ----------------------------------------------------------------
// ------------------------ LITERALS ------------------------------
// ----------------------------------------------------------------
typedef struct IntegerLiteral {
  Expression base; // EXPR_INT
  Token token;
  int64_t value;
} IntegerLiteral;

/* Allocates a new Program using the arena allocator with the provided capcity */
Program *ast_program_new(Arena *arena, size_t capacity);

/* Allocates a new LetStatement using the arena allocator */
LetStatement *ast_let_new(Arena *, Token token, Identifier *name, Expression *value);

/* Allocates a new Identifier using the arena allocator */
Identifier *ast_ident_new(Arena *, Token token, StrView value);

/* Allocates a new IntegerLiteral using the arena allocator */
IntegerLiteral *ast_int_new(Arena *, Token token, int64_t value);

#endif // YOKAI_AST_H
