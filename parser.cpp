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


uint32_t
get_function_index(char name[2])
{
  uint32_t result;

  if (name[0] <= 'Z')
  {
    result = name[0] - 'A';
  }
  else
  {
    result = name[0] - 'a';
  }
  result *= ((26 * 2) + 10);  // The number of combinations in the second digit.

  if (name[1] <= '9')
  {
    result += name[1] - '0';
  }
  else if (name[1] <=  'Z')
  {
    result += name[1] - 'A';
  }
  else
  {
    result += name[1] - 'a';
  }

  assert(result < MAX_FUNCTIONS);
  return result;
}


Maze *
parse(GameMemory * game_memory, const char * filename)
{
  // TODO: How to get it the right way up?!
  // TODO: Deal with gaps.

  // TODO: Also, don't forget it isn't parsing tree-big properly...
  // TODO: IMPORTANT: It can split a cell in two stupid! This parsing function
  //                    really needs unstupiding. >:(

  FILE * file = fopen(filename, "r");
  assert(file != NULL);

  Maze * maze = take_struct_mem(game_memory, Maze, 1);
  maze->tree.bounds = (Rectangle){(V2){0, 0}, (V2){10000, 10000}};
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
      else if (isLetter(potential_cell[0]) && (isLetter(potential_cell[1]) ||
                                               isNum(potential_cell[1])))
      {
        uint32_t function_index = get_function_index(potential_cell);
        uint32_t function_parse_bytes_read = 0;
        if ((sscanf((buffer + offset + bytes_read), " -> %n", &function_parse_bytes_read) != EOF) && (function_parse_bytes_read > 0))
        {
          Function * function = maze->functions + function_index;
          function->type = FunctionAssignment;
          function->name[0] = potential_cell[0];
          function->name[1] = potential_cell[1];

          bytes_read += function_parse_bytes_read;
        }
        else
        {
          new_cell.type = CELL_FUNCTION;
          new_cell.data.function_index = function_index;
        }
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

        uint32_t digit0 = potential_cell[0] - '0';
        uint32_t digit1 = potential_cell[1] - '0';
        new_cell.data.pause = (10 * digit0) + digit1;
      }

      if (new_cell.type == CELL_NULL)
      {
        if (bytes_read <= 1)
        {
          // We must have reached the end of the file: there are no
          //   more characters OR whitespace.
          break;
        }
        // If we haven't found a valid cell only move on by one char
        //   instead of two.
        offset += bytes_read - 1;
      }
      else
      {
        Cell * cell = get_cell(maze, x, y, game_memory);
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
  // TODO: IMPORTANT: This is still broken! >:(
  // TODO: IMPORTANT: This is complete rubbish! It gets an extra line
  //                  sometimes!! WHY?!?
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