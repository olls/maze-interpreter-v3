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


QuadTree *
create_tree(GameMemory * game_memory, Rectangle bounds)
{
  QuadTree * tree = 0;
  if (game_memory)
  {
    tree = (QuadTree *)take_struct_mem(game_memory, QuadTree, 1);
    tree->bounds = bounds;
  }
  else
  {
    assert(!"Cell doesn't exist yet, need to pass game_memory to create new one!");
  }
  return tree;
}


Cell *
get_cell(Maze * maze, uint32_t x, uint32_t y, GameMemory * game_memory = 0)
{
  QuadTree * tree = &(maze->tree);

  Cell * cell;
  while (!(cell = get_cell(tree, x, y)))
  {
    Rectangle top_right_bounds    = get_top_right(tree->bounds);
    Rectangle top_left_bounds     = get_top_left(tree->bounds);
    Rectangle bottom_right_bounds = get_bottom_right(tree->bounds);
    Rectangle bottom_left_bounds  = get_bottom_left(tree->bounds);

    if (in_rectangle((V2){x, y}, top_right_bounds))
    {
      if (!tree->top_right)
      {
        tree->top_right = create_tree(game_memory, top_right_bounds);
      }
      tree = tree->top_right;
    }
    else if (in_rectangle((V2){x, y}, top_left_bounds))
    {
      if (!tree->top_left)
      {
        tree->top_left = create_tree(game_memory, top_left_bounds);
      }
      tree = tree->top_left;
    }
    else if (in_rectangle((V2){x, y}, bottom_right_bounds))
    {
      if (!tree->bottom_right)
      {
        tree->bottom_right = create_tree(game_memory, bottom_right_bounds);
      }
      tree = tree->bottom_right;
    }
    else if (in_rectangle((V2){x, y}, bottom_left_bounds))
    {
      if (!tree->bottom_left)
      {
        tree->bottom_left = create_tree(game_memory, bottom_left_bounds);
      }
      tree = tree->bottom_left;
    }
    else
    {
      // NOTE: If it gets here: it is in old-tree but not any of it's
      //       subdivisions
      invalid_code_path;
    }
  }
  return cell;
}
