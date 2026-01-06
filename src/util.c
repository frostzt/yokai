/*
 * util.c - Utilities
 *
 * Author: frostzt
 * Date: 2026-01-07
 */

#include <stddef.h>

#include "yokai/util.h"

size_t max(size_t a, size_t b) {
  return a > b ? a : b;
}
