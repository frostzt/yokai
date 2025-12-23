#include "yokai/token.h"

TokenType lookup_ident(StrView ident) {
  if (sv_eq_cstr(ident, "fn")) return TOK_FUNCTION;
  if (sv_eq_cstr(ident, "let")) return TOK_LET;
  if (sv_eq_cstr(ident, "true")) return TOK_TRUE;
  if (sv_eq_cstr(ident, "false")) return TOK_FALSE;
  if (sv_eq_cstr(ident, "if")) return TOK_IF;
  if (sv_eq_cstr(ident, "else")) return TOK_ELSE;
  if (sv_eq_cstr(ident, "return")) return TOK_RETURN;
  return TOK_IDENT;
}

const char *token_type_name(TokenType t) {
  switch (t) {
  case TOK_FUNCTION:
    return "fn";
  case TOK_LET:
    return "let";
  case TOK_TRUE:
    return "true";
  case TOK_FALSE:
    return "false";
  case TOK_IF:
    return "if";
  case TOK_ELSE:
    return "else";
  case TOK_RETURN:
    return "return";
  default:
    return NULL;
  }
}
