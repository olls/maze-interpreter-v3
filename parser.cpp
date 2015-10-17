bool
isNum(char character)
{
  bool result = (character >= '0') && (character <= '9');
  return result;
}


bool
isLetter(char character)
{
  bool result = (((character >= 'a') && (character <= 'z')) ||
                 ((character >= 'A') && (character <= 'Z')));
  return result;
}


Maze *
parse(GameMemory * game_memory, const char * filename)
{
  // TODO: How to get it the right way up?!

  FILE * file = fopen(filename, "r");
  assert(file != NULL);

  Maze * maze = take_struct_mem(game_memory, Maze, 1);
  // maze->tree.bounds = (Rectangle){(V2){0, 0}, (V2){50, 50}};
  maze->tree.bounds = (Rectangle){(V2){0, 0}, (V2){FLT_MAX, FLT_MAX}};
  maze->tree.used = 0;
  maze->width = 0;
  maze->height = 0;

  uint32_t x = 0;
  uint32_t y = 0;

  char buffer[256];
  while (fgets(buffer, sizeof(buffer), file) != NULL)
  {

    char potential_cell[3];
    size_t offset = 0;
    uint32_t bytes_read = 0;
    while (sscanf((buffer + offset), "%2s %n", potential_cell, &bytes_read) != EOF)
    {
      Cell new_cell = {};
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
      else if (((potential_cell[0] == '#') && (potential_cell[1] == '#')) ||
               ((potential_cell[0] == '`') && (potential_cell[1] == '`')))
      {
        new_cell.type = CELL_WALL;
      }
      else if ((potential_cell[0] == '(') && (potential_cell[1] == ')'))
      {
        new_cell.type = CELL_HOLE;
      }
      else if ((potential_cell[0] == '<') && (potential_cell[1] == '>'))
      {
        new_cell.type = CELL_SPLITTER;
      }
      else if (isLetter(potential_cell[0]) && isLetter(potential_cell[1]))
      {
        new_cell.type = CELL_FUNCTION;
      }
      else if ((potential_cell[0] == '-') && (potential_cell[1] == '-'))
      {
        new_cell.type = CELL_ONCE;
      }
      else if ((potential_cell[0] == '*') && (potential_cell[1] == '*'))
      {
        new_cell.type = CELL_SIGNAL;
      }
      else if ((potential_cell[0] == '>') && (potential_cell[1] == '>'))
      {
        new_cell.type = CELL_INC;
      }
      else if ((potential_cell[0] == '<') && (potential_cell[1] == '<'))
      {
        new_cell.type = CELL_DEC;
      }
      else if ((potential_cell[0] == '%') && ((potential_cell[1] == 'U') ||
                                              (potential_cell[1] == 'u')))
      {
        new_cell.type = CELL_UP;
      }
      else if ((potential_cell[0] == '%') && ((potential_cell[1] == 'D') ||
                                              (potential_cell[1] == 'd')))
      {
        new_cell.type = CELL_DOWN;
      }
      else if ((potential_cell[0] == '%') && ((potential_cell[1] == 'L') ||
                                              (potential_cell[1] == 'l')))
      {
        new_cell.type = CELL_LEFT;
      }
      else if ((potential_cell[0] == '%') && ((potential_cell[1] == 'R') ||
                                              (potential_cell[1] == 'r')))
      {
        new_cell.type = CELL_RIGHT;
      }
      else if (isNum(potential_cell[0]) && isNum(potential_cell[1]))
      {
        new_cell.type = CELL_PAUSE;
        new_cell.data = 0;
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
        Cell * cell = get_cell(maze, x, y, game_memory);

        // NOTE: Doing them individually to avoid overwriting the x, y set in get_cell.
        cell->type = new_cell.type;
        cell->data = new_cell.data;

        offset += bytes_read;
        ++x;

        printf("%s ", potential_cell);
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
      ++y;

      if (x > maze->width)
      {
        maze->width = x;
      }
      x = 0;
    }
  }

  // Account for last line
  printf("\n");
  maze->height = y + 1;

  if (x > maze->width)
  {
    maze->width = x;
  }
  x = 0;

  printf("(%d, %d)\n", maze->width, maze->height);

  fclose(file);
  return maze;
}