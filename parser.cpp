Maze *
parse(GameMemory * game_memory, char * filename)
{

  FILE * file = fopen(filename, "r");
  assert(file != NULL);

  Maze * maze = take_struct_mem(game_memory, Maze, 1);
  MazeBlock * first_block = take_struct_mem(game_memory, MazeBlock, 1);
  maze->width = 0;
  maze->height = 0;
  maze->used = 0;
  maze->first_block = first_block;

  uint32_t current_width_in_cells = 0;

  char buffer[256];
  while (fgets(buffer, sizeof(buffer), file) != NULL)
  {

    char potential_cell[3];
    size_t offset = 0;
    uint32_t bytes_read = 0;
    while (sscanf((buffer + offset), "%2s %n", potential_cell, &bytes_read) != EOF)
    {
      Cell new_cell;
      new_cell.type = CELL_NULL;

      if ((potential_cell[0] == '^') &&
          (potential_cell[1] == '^'))
      {
        new_cell.type = CELL_START;
      }
      else if ((potential_cell[0] == '.') &&
               (potential_cell[1] == '.'))
      {
        new_cell.type = CELL_PATH;
      }
      else if (((potential_cell[0] == '#') &&
                (potential_cell[1] == '#')) ||
               ((potential_cell[0] == '`') &&
                (potential_cell[1] == '`')))
      {
        new_cell.type = CELL_WALL;
      }
      else if ((potential_cell[0] == '(') &&
               (potential_cell[1] == ')'))
      {
        new_cell.type = CELL_HOLE;
      }
      else if ((potential_cell[0] == '<') &&
               (potential_cell[1] == '>'))
      {
        new_cell.type = CELL_SPLITTER;
      }
      else if ((potential_cell[0] > 'A') &&
               (potential_cell[1] == '#'))
      {
        new_cell.type = CELL_FUNCTION;
      }
      else if ((potential_cell[0] == '-') &&
               (potential_cell[1] == '-'))
      {
        new_cell.type = CELL_ONCE;
      }
      else if ((potential_cell[0] == '%') &&
               ((potential_cell[1] == 'U') ||
                (potential_cell[1] == 'u')))
      {
        new_cell.type = CELL_UP;
      }
      else if ((potential_cell[0] == '%') &&
               ((potential_cell[1] == 'D') ||
                (potential_cell[1] == 'd')))
      {
        new_cell.type = CELL_DOWN;
      }
      else if ((potential_cell[0] == '%') &&
               ((potential_cell[1] == 'L') ||
                (potential_cell[1] == 'l')))
      {
        new_cell.type = CELL_LEFT;
      }
      else if ((potential_cell[0] == '%') &&
               ((potential_cell[1] == 'R') ||
                (potential_cell[1] == 'r')))
      {
        new_cell.type = CELL_RIGHT;
      }

      if (new_cell.type == CELL_NULL)
      {
        if (bytes_read <= 1)
        {
          // We must have reached the end of the file: there are no more characters OR whitespace.
          break;
        }
        // If we havent't found a valid cell only move on by one char instead of two.
        offset += bytes_read - 1;
      }
      else
      {
        offset += bytes_read;
        ++current_width_in_cells;

        printf("%s ", potential_cell);

        if (maze->used >= array_count(maze->first_block->cells))
        {
          // First block is full
          MazeBlock * new_block = take_struct_mem(game_memory, MazeBlock, 1);

          *new_block = *(maze->first_block);
          maze->first_block->next = new_block;
          maze->used = 0;
        }

        Cell * cell = maze->first_block->cells + maze->used++;
        *cell = new_cell;
      }
    }

    size_t ln = sizeof(buffer);
    while (ln > 0 && buffer[ln - 1] != '\n')
    {
      --ln;
    }

    if (ln > 0)
    {
      printf("\n");
      ++maze->height;

      if (current_width_in_cells > maze->width)
      {
        maze->width = current_width_in_cells;
      }
      current_width_in_cells = 0;
    }
  }

  // Account for last line
  printf("\n");
  ++maze->height;

  if (current_width_in_cells > maze->width)
  {
    maze->width = current_width_in_cells;
  }
  current_width_in_cells = 0;

  printf("(%d, %d)\n", maze->width, maze->height);

  fclose(file);
  return maze;
}