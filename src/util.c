/*
 * util.c - Utilities
 *
 * Author: frostzt
 * Date: 2026-01-07
 */

#include "yokai/util.h"

size_t max(size_t a, size_t b) {
  return a > b ? a : b;
}

bool safe_parse_int64_sv(const StrView *sv, int64_t *out) {
  size_t len = sv->len;

  /* copy into a temporary buffer */
  char buf[len + 1];
  memcpy(buf, sv->data, len);
  buf[len] = '\0';

  char *end;
  int64_t value = strtoll(buf, &end, 10);
  if (end == buf || *end != '\0' || errno == ERANGE) { return false; }

  *out = value;
  return true;
}
