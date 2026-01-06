/*
 * strbuf.h - String builder
 *
 * Author: frostzt
 * Date: 2026-01-07
 */

#include "yokai/arena.h"
#include "yokai/str.h"

typedef struct StrBuf {
  Arena *arena;
  char *buf;
  size_t len;
  size_t cap;
} StrBuf;

/* initializes a string buffer and allocates initial capacity */
void sb__init(StrBuf *sb, size_t initial_cap);

/* ensures that the string buffer has the capacity needed */
void sb__ensure(StrBuf *sb, size_t need);

/* appends a string into string buffer */
void sb__append(StrBuf *sb, const char *s, size_t n);

/* appends a char into string buffer */
void sb__putc(StrBuf *sb, char ch);

/* appends a StrView's data into string buffer */
void sb__append_strview(StrBuf *sb, StrView sv);

/* appends a c-string into string buffer */
void sb__append_cstr(StrBuf *sb, const char *s);

/* returns a const reference to the underlying string */
const char *sb__cstr(const StrBuf *sb);

/* returns the underlying string as a StrView */
StrView sb__view(const StrBuf *sb);
