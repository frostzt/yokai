#ifndef YOKAI_STR_H
#define YOKAI_STR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  const char *data;
  size_t len;
} StrView;

/* Create a view from a c-string */
StrView sv_from_cstr(const char *s);

/* Compare two views */
bool sv_eq(StrView a, StrView b);

/* Compare view to a c-string literal */
bool sv_eq_cstr(StrView a, const char *b);

/* Hash a view */
uint64_t sv_hash(StrView s);

#endif // YOKAI_STR_H
