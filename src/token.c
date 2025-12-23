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
  case TOK_ILLEGAL:
    return "ILLEGAL";
  case TOK_EOF:
    return "EOF";
  case TOK_IDENT:
    return "IDENT";
  case TOK_INT:
    return "INT";
  case TOK_EQ:
    return "==";
  case TOK_NOT_EQ:
    return "!=";
  case TOK_SEMICOLON:
    return ";";
  case TOK_COMMA:
    return ",";
  case TOK_ASSIGN:
    return "=";
  case TOK_PLUS:
    return "+";
  case TOK_MINUS:
    return "-";
  case TOK_BANG:
    return "!";
  case TOK_ASTERISK:
    return "*";
  case TOK_SLASH:
    return "/";
  case TOK_LT:
    return "<";
  case TOK_GT:
    return ">";
  case TOK_LPAREN:
    return "(";
  case TOK_RPAREN:
    return ")";
  case TOK_LBRACE:
    return "{";
  case TOK_RBRACE:
    return "}";
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
