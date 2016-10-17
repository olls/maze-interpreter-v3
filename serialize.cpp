const u32 CELL_LENGTH = 3;
const u32 LINE_BREAK_LENGTH = 1;


void
find_min_max(QuadTree *tree, V2 *most_left, V2 *most_top, V2 *most_right, V2 *most_bottom)
{
  if (tree)
  {
    for (u32 cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell *cell = tree->cells + cell_index;

      if (cell->x < most_left->x)
      {
        *most_left = (V2){cell->x, cell->y};
      }
      if (cell->y < most_top->y)
      {
        *most_top = (V2){cell->x, cell->y};
      }
      if (cell->x > most_right->x)
      {
        *most_right = (V2){cell->x, cell->y};
      }
      if (cell->y > most_bottom->y)
      {
        *most_bottom = (V2){cell->x, cell->y};
      }
    }

    find_min_max(tree->top_left, most_left, most_top, most_right, most_bottom);
    find_min_max(tree->top_right, most_left, most_top, most_right, most_bottom);
    find_min_max(tree->bottom_left, most_left, most_top, most_right, most_bottom);
    find_min_max(tree->bottom_right, most_left, most_top, most_right, most_bottom);
  }
}


void
write_cells(QuadTree *tree, char *file, u32 width, u32 height, V2 maze_origin)
{
  if (tree)
  {
    for (u32 cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell *cell = tree->cells + cell_index;

      u32 rel_cell_pos_x = (u32)((s32)cell->x - (s32)maze_origin.x);
      u32 rel_cell_pos_y = (u32)((s32)cell->y - (s32)maze_origin.y);

      u32 n_line_breaks = rel_cell_pos_y*LINE_BREAK_LENGTH;

      u32 cell_file_offset = CELL_LENGTH*(rel_cell_pos_x + (rel_cell_pos_y * width)) + n_line_breaks;
      char *file_pos = file + cell_file_offset;

      if (cell->type == CELL_FUNCTION)
      {
        // TODO!
        file_pos[0] = 'A';
        file_pos[1] = 'A';
      }
      else if (cell->type == CELL_PAUSE)
      {
        // TODO!
        file_pos[0] = '0';
        file_pos[1] = '0';
      }
      else
      {
        const char *text = CELL_TYPE_TXT[cell->type];
        memcpy(file_pos, text, 2);
      }

      file_pos[2] = ' ';

      log(L_Serializer, "%d,%d - %d %.3s", rel_cell_pos_x, rel_cell_pos_y, cell_file_offset, file_pos);
    }

    write_cells(tree->top_left, file, width, height, maze_origin);
    write_cells(tree->top_right, file, width, height, maze_origin);
    write_cells(tree->bottom_left, file, width, height, maze_origin);
    write_cells(tree->bottom_right, file, width, height, maze_origin);
  }
}


V2
get_maze_size(Maze *maze)
{
  V2 most_left   = {maze->tree.cells[maze->tree.used-1].x, maze->tree.cells[maze->tree.used-1].y};
  V2 most_top    = most_left;
  V2 most_right  = most_left;
  V2 most_bottom = most_left;

  find_min_max(&maze->tree, &most_left, &most_top, &most_right, &most_bottom);

  V2 result = {(u32)(most_right - most_left).x + 1,
               (u32)(most_bottom - most_top).y + 1};

  return result;
}


void
serialize_maze(Maze *maze, const char *filename)
{
  // TMP: For testing without overwritting
#if 1
  ((char *)filename)[0] = '_';
#endif

  V2 most_left   = {maze->tree.cells[maze->tree.used-1].x, maze->tree.cells[maze->tree.used-1].y};
  V2 most_top    = most_left;
  V2 most_right  = most_left;
  V2 most_bottom = most_left;

  find_min_max(&maze->tree, &most_left, &most_top, &most_right, &most_bottom);

  u32 width  = (u32)(most_right - most_left).x + 1;
  u32 height = (u32)(most_bottom - most_top).y + 1;

  u32 length = CELL_LENGTH * width * height +
               LINE_BREAK_LENGTH * height;

  u32 fd = open(filename, O_RDWR | O_TRUNC);
  if (fd == -1)
  {
    printf("Failed to open file for saving.\n");
    exit(1);
  }

  if (ftruncate(fd, length) == -1)
  {
    printf("Failed to open file for saving.\n");
    exit(1);
  }

  char *file = (char *)mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd, 0);
  if (file == MAP_FAILED)
  {
    printf("Failed to open file for saving.\n");
    exit(1);
  }

  memset(file, ' ', length);
  write_cells(&maze->tree, file, width, height, (V2){most_left.x, most_top.y});

  // Add line breaks
  for (u32 line = 0;
       line < height;
       ++line)
  {
    u32 file_offset = (line+1) * width * CELL_LENGTH + line;
    char *file_pos = file + file_offset;
    *file_pos = '\n';
    log(L_Serializer, "%d", file_offset);
  }

  log_s(L_Serializer, "%s", file);

  if (munmap((void *)file, length) != 0)
  {
    printf("Error unmapping file.");
  }

  if (close(fd) != 0)
  {
    printf("Error while closing file descriptor.");
  }
}

