MazeBlock *
get_block(Maze * maze, uint32_t block_x, uint32_t block_y)
{
  // TODO: Better hash function
  // TODO: Deal with collisions
  uint32_t hash_value = (BLOCK_NUM - 1) & ((block_x * 3) + (block_y * 7));

  MazeBlock * block = maze->hash + hash_value;

  return block;
}


Cell *
get_cell(Maze * maze, uint32_t cell_x, uint32_t cell_y)
{
  uint32_t block_x = cell_x / BLOCK_SIDE_LENGTH;
  uint32_t block_y = cell_y / BLOCK_SIDE_LENGTH;

  uint32_t cell_x_in_block = cell_x - (block_x * BLOCK_SIDE_LENGTH);
  uint32_t cell_y_in_block = cell_y - (block_y * BLOCK_SIDE_LENGTH);

  MazeBlock * block = get_block(maze, block_x, block_y);
  Cell * cell = block->cells + cell_x_in_block + (cell_y_in_block * BLOCK_SIDE_LENGTH);

  return cell;
}
