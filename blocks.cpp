MazeBlock *
get_block(GameMemory * game_memory, Maze * maze, V2 block_pos)
{
  uint32_t block_x = (uint32_t)block_pos.x;
  uint32_t block_y = (uint32_t)block_pos.y;

  // TODO: Better hash function
  // uint32_t hash_value = (BLOCK_NUM - 1) & ((block_x * 3) + (block_y * 7));
  // uint32_t hash_value = (BLOCK_NUM - 1) & ((block_x << 3) + (block_y << 7));
  // uint32_t hash_value = (BLOCK_NUM - 1) & ((block_x >> 3) + (block_y >> 7));
  uint32_t hash_value = (BLOCK_NUM - 1) & ((block_x * 0x1f1f1f1f) ^ block_y);
  // uint32_t hash_value = (BLOCK_NUM - 1) & ((block_x * 7) + ((block_y * 31) + 1));

  MazeBlock * block = maze->hash + hash_value;

  if (block->used)
  {
    // Collision, get next or new block
    while (!(block->x == block_x && block->y == block_y))
    {
      if (block->next)
      {
        block = block->next;
      }
      else
      {
#ifdef DEBUG
        ++maze->collisions;
#endif
        MazeBlock * new_block = take_struct_mem(game_memory, MazeBlock, 1);
        block->next = new_block;
        block = new_block;

        break;
      }
    }
  }

  block->used = true;
  block->x = block_x;
  block->y = block_y;

  return block;
}


Cell *
get_cell(GameMemory * game_memory, Maze * maze, V2 cell_pos)
{
  V2 block_pos = round_down(cell_pos / BLOCK_SIDE_LENGTH);
  V2 cell_pos_in_block = cell_pos - (block_pos * BLOCK_SIDE_LENGTH);

  MazeBlock * block = get_block(game_memory, maze, block_pos);
  Cell * cell = block->cells + (uint32_t)(cell_pos_in_block.x + (cell_pos_in_block.y * BLOCK_SIDE_LENGTH));

  return cell;
}
