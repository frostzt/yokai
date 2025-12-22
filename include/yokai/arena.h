#ifndef ARENA_ASSERT
#include <assert.h>
#define ARENA_ASSERT assert
#endif // ARENA_ASSERT

#ifndef YOKAI_ARENA_H
#define YOKAI_ARENA_H

#include <stddef.h>
#include <stdint.h>

typedef struct ArenaBlock {
  uint8_t *data;
  size_t used;
  size_t capacity;
  struct ArenaBlock *next;
} ArenaBlock;

typedef struct Arena {
  ArenaBlock *head;
  size_t block_size;
} Arena;

/* Create an arena with blocks of `block_size` bytes */
Arena arena_create(size_t block_size);

/* Free all memory owned by the arena */
void arena_destroy(Arena* arena);

/* Allocate `size` bytes with `align` alignment */
void* arena_alloc(Arena* arena, size_t size, size_t align);

/* Allocate and copy a string */
char* arena_strdup(Arena* arena, const char* src, size_t len);

void arena_reset(Arena* arena);

#endif // YOKAI_ARENA_H
