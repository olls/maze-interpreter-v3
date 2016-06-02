Cell *
get_cell(QuadTree * tree, u32 x, u32 y)
{
  Cell *cell = 0;
  for (u32 cell_index = 0;
       cell_index < tree->used;
       ++cell_index)
  {
    Cell *test_cell = tree->cells + cell_index;
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
create_tree(Memory *memory, Rectangle bounds)
{
  QuadTree * tree = 0;
  if (memory)
  {
    tree = push_struct(memory, QuadTree);
    tree->bounds = bounds;
  }
  return tree;
}


Cell **
get_cell_from_hash(Maze *maze, u32 x, u32 y)
{
  u32 hash = (7 * x + 13 * y) % CELL_CACHE_SIZE;
  Cell **hash_slot = maze->cache_hash + hash;

  return hash_slot;
}


Cell *
get_cell(Maze *maze, u32 x, u32 y, Memory *memory = 0)
{
  QuadTree * tree = &(maze->tree);

  Cell *cell = 0;
  Cell *hash_cell = *get_cell_from_hash(maze, x, y);

  if (hash_cell && hash_cell->x == x && hash_cell->y == y)
  {
    cell = hash_cell;
    printf("Found\n");
  }
  else
  {
    printf("Not Found\n");
    while (tree && !(cell = get_cell(tree, x, y)))
    {
      Rectangle top_right_bounds    = get_top_right(tree->bounds);
      Rectangle top_left_bounds     = get_top_left(tree->bounds);
      Rectangle bottom_right_bounds = get_bottom_right(tree->bounds);
      Rectangle bottom_left_bounds  = get_bottom_left(tree->bounds);

      if (in_rectangle((V2){x, y}, top_right_bounds))
      {
        if (!tree->top_right)
        {
          tree->top_right = create_tree(memory, top_right_bounds);
        }
        tree = tree->top_right;
      }
      else if (in_rectangle((V2){x, y}, top_left_bounds))
      {
        if (!tree->top_left)
        {
          tree->top_left = create_tree(memory, top_left_bounds);
        }
        tree = tree->top_left;
      }
      else if (in_rectangle((V2){x, y}, bottom_right_bounds))
      {
        if (!tree->bottom_right)
        {
          tree->bottom_right = create_tree(memory, bottom_right_bounds);
        }
        tree = tree->bottom_right;
      }
      else if (in_rectangle((V2){x, y}, bottom_left_bounds))
      {
        if (!tree->bottom_left)
        {
          tree->bottom_left = create_tree(memory, bottom_left_bounds);
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

    // Add to hash cache
    if (cell)
    {
      Cell **hash_slot = get_cell_from_hash(maze, x, y);
      *hash_slot = cell;
    }
  }

  return cell;
}