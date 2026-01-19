/*
 * util.h - Utilities
 *
 * Author: frostzt
 * Date: 2026-01-07
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "yokai/str.h"

/* does what you expect it to */
size_t max(size_t a, size_t b);

/* parses a string view into int64 */
bool safe_parse_int64_sv(const StrView *sv, int64_t *out);
