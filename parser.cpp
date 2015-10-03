void
parse()
{
  char filename[] = "test.mz";

  FILE * file = fopen(filename, "r");
  assert(file != NULL);

  uint32_t width_in_cells = 0;
  uint32_t height_in_cells = 0;

  char buffer[256];
  while (fgets(buffer, sizeof(buffer), file) != NULL)
  {

    char potential_cell[3];
    size_t offset = 0;
    while (sscanf((buffer + offset), "%2s", potential_cell) != EOF)
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
        offset += 1;
      }
      else
      {
        offset += 2;


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
      height_in_cells += 1;
      printf("\n");
    }

  }

  fclose(file);
}