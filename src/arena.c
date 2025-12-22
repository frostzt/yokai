#include <stdlib.h>

#include "yokai/arena.h"

Arena arena_create(size_t block_size) {
  Arena ar = {0};
  ar.block_size = block_size;

  ArenaBlock *block = (ArenaBlock *)malloc(sizeof(ArenaBlock));
  ARENA_ASSERT(block);

  block->data = (uint8_t *)malloc(block_size);
  ARENA_ASSERT(block->data);

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
