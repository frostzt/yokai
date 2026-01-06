/*
 * strbuf.c - String builder
 *
 * Author: frostzt
 * Date: 2026-01-07
 */

#include <stdalign.h>
#include <string.h>

#include "yokai/arena.h"
#include "yokai/strbuf.h"
#include "yokai/util.h"

void sb__init(StrBuf *sb, size_t initial_cap) {
  size_t cap = initial_cap <= 0 ? 8 : initial_cap;
  sb->len = 0;
  sb->cap = cap;

  sb->buf = arena_alloc(sb->arena, cap, 1);
  sb->buf[0] = '\0';
}

void sb__ensure(StrBuf *sb, size_t need) {
  if (need <= sb->cap) return;

  size_t new_cap = max(need, 2 * sb->cap);
  /* preventive guard against (sb->cap * 2) overflow */
  if (sb->cap > SIZE_MAX / 2) { new_cap = need; }

  char *new_buf = arena_alloc(sb->arena, new_cap, 1);
  memcpy(new_buf, sb->buf, sb->len + 1);

  sb->buf = new_buf;
  sb->cap = new_cap;
  sb->buf[sb->len] = '\0';
}

void sb__append(StrBuf *sb, const char *s, size_t n) {
  size_t need = sb->len + n + 1;
  sb__ensure(sb, need);

  memcpy(sb->buf + sb->len, s, n);
  sb->len += n;
  sb->buf[sb->len] = '\0';
}

void sb__putc(StrBuf *sb, char ch) {
  size_t need = sb->len + 2;
  sb__ensure(sb, need);

  sb->buf[sb->len] = ch;
  sb->len++;
  sb->buf[sb->len] = '\0';
}

void sb__append_strview(StrBuf *sb, StrView sv) {
  size_t need = sb->len + sv.len + 1;
  sb__ensure(sb, need);

  memcpy(sb->buf + sb->len, sv.data, sv.len);
  sb->len += sv.len;
  sb->buf[sb->len] = '\0';
}

void sb__append_cstr(StrBuf *sb, const char *s) {
  size_t str_length = strlen(s);
  sb__append(sb, s, str_length);
}

const char *sb__cstr(const StrBuf *sb) {
  return sb->buf;
}

StrView sb__view(const StrBuf *sb) {
  return (StrView){sb->buf, sb->len};
}
