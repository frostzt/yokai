#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "yokai/str.h"

#define FNV_1A_64_PRIME 0x100000001b3ULL
#define FNV_1A_64_OFFSET_BASIS 0xcbf29ce484222325ULL

StrView sv_from_cstr(const char *s) {
  StrView sv;
  if (s == NULL) {
    sv.data = NULL;
    sv.len = 0;
    return sv;
  }

  sv.data = s;
  sv.len = strlen(s);
  return sv;
}

bool sv_eq(StrView a, StrView b) {
  if (a.len != b.len) {
    return false;
  }

  if (a.len == 0) {
    return true;
  }
  
  if (a.len > 0) {
    assert(a.data != NULL);
    assert(b.data != NULL);
  }

  int res = memcmp(a.data, b.data, a.len);
  return res == 0;
}

bool sv_eq_cstr(StrView a, const char *b) {
  if (b == NULL) {
    return false;
  }

  if (a.len != strlen(b)) {
    return false;
  }

  if (a.len == 0) {
    return true;
  }

  if (a.len > 0) {
    assert(a.data != NULL);
  }

  int res = memcmp(a.data, b, a.len);
  return res == 0;
}

uint64_t sv_hash(StrView s) {
  if (s.len == 0) {
    return FNV_1A_64_OFFSET_BASIS;
  } else {
    assert(s.data != NULL);
  }

  uint64_t hash = FNV_1A_64_OFFSET_BASIS;

  const unsigned char *bp = (const unsigned char *)s.data;
  const unsigned char *be = bp + s.len;

  while (bp < be) {
    hash ^= (uint64_t)*bp++;
    hash *= FNV_1A_64_PRIME;
  }

  return hash;
}
