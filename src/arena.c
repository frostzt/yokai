#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "yokai/arena.h"

Arena arena_create(size_t block_size) {
  Arena ar = {0};
  ar.block_size = block_size;

  ArenaBlock *block = (ArenaBlock *)malloc(sizeof(ArenaBlock));
  assert(block);

  block->data = (uint8_t *)malloc(block_size);
  assert(block->data);

  block->used = 0;
  block->capacity = block_size;
  block->next = NULL;

  ar.head = block;
  return ar;
}

void arena_destroy(Arena *arena) {
  if (!arena) {
    return;
  }

  ArenaBlock *blk = arena->head;
  while (blk) {
    ArenaBlock *next = blk->next;
    free(blk->data);
    free(blk);
    blk = next;
  }

  arena->head = NULL;
  arena->block_size = 0;
}

void *arena_alloc(Arena *arena, size_t size, size_t align) {
  if (!arena || size == 0) {
    return NULL;
  }

  assert(align != 0);
  assert(is_power_of_two(align));

  ArenaBlock *blk = arena->head;
  assert(blk);

  // try current head block first
  size_t start = align_up(blk->used, align);
  if (start + size <= blk->capacity) {
    void *ptr = blk->data + start;
    blk->used = start + size;
    return ptr;
  }

  // need a new block
  size_t new_cap = arena->block_size;
  size_t min_needed = size + (align - 1);
  if (new_cap < min_needed) {
    new_cap = min_needed;
  }

  ArenaBlock *new_blk = (ArenaBlock *)malloc(sizeof(ArenaBlock));
  assert(new_blk);

  new_blk->data = (uint8_t *)malloc(new_cap);
  assert(new_blk->data);

  new_blk->used = 0;
  new_blk->capacity = new_cap;
  new_blk->next = blk;

  arena->head = new_blk;

  // allocate from new head
  start = align_up(arena->head->used, align);
  assert(start + size <= arena->head->capacity);

  void *ptr = arena->head->data + start;
  arena->head->used = start + size;
  return ptr;
}

char *arena_strdup(Arena *arena, const char *src, size_t len) {
  if (!arena || !src) {
    return NULL;
  }

  char *cPtr = (char *)arena_alloc(arena, len + 1, 1);
  assert(cPtr != NULL);

  memcpy(cPtr, src, len);
  cPtr[len] = '\0';
  return cPtr;
}

void arena_reset(Arena *arena) {
  if (!arena || !arena->head) {
    return;
  }

  ArenaBlock *head = arena->head;
  head->used = 0;

  ArenaBlock *blk = head->next;
  while (blk) {
    ArenaBlock *next = blk->next;
    free(blk->data);
    free(blk);
    blk = next;
  }

  head->next = NULL;
}
