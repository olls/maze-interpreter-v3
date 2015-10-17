Cell *
get_cell(QuadTree * tree, uint32_t x, uint32_t y)
{
  Cell * cell = 0;
  for (uint32_t cell_index = 0;
       cell_index < tree->used;
       ++cell_index)
  {
    Cell * test_cell = tree->cells + cell_index;
    if ((test_cell->x == x) &&
        (test_cell->y == y))
    {
      cell = test_cell;
      break;
    }
  }
  if (!cell && tree->used != QUAD_STORE_N)
  {
    cell = tree->cells + tree->used++;
    cell->x = x;
    cell->y = y;
  }
  return cell;
}


Cell *
get_cell(Maze * maze, uint32_t x, uint32_t y, GameMemory * game_memory = 0)
{
  QuadTree * tree = &(maze->tree);

  Cell * cell;
  while (!(cell = get_cell(tree, x, y)))
  {
    if (!tree->top_right)
    {
      if (!game_memory)
      {
        return 0;
      }

      QuadTree * trees = take_struct_mem(game_memory, QuadTree, 4);
      tree->top_right    = trees++;
      tree->top_left     = trees++;
      tree->bottom_right = trees++;
      tree->bottom_left  = trees;

      tree->top_right->bounds    = get_top_right(tree->bounds);
      tree->top_left->bounds     = get_top_left(tree->bounds);
      tree->bottom_right->bounds = get_bottom_right(tree->bounds);
      tree->bottom_left->bounds  = get_bottom_left(tree->bounds);

#ifdef DEBUG
      ++maze->subdivisions;
#endif

    }

    if (in_rectangle((V2){x, y}, tree->top_right->bounds))
    {
      tree = tree->top_right;
      continue;
    }
    if (in_rectangle((V2){x, y}, tree->top_left->bounds))
    {
      tree = tree->top_left;
      continue;
    }
    if (in_rectangle((V2){x, y}, tree->bottom_right->bounds))
    {
      tree = tree->bottom_right;
      continue;
    }
    if (in_rectangle((V2){x, y}, tree->bottom_left->bounds))
    {
      tree = tree->bottom_left;
      continue;
    }

    // NOTE: If it gets here: it is in old-tree but not any of it's
    //       subdivisions
    invalid_code_path;
  }
  return cell;
}
