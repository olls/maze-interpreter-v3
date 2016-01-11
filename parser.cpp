#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


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

  Maze * maze = take_struct_mem(game_memory, Maze, 1);
  maze->tree.bounds = (Rectangle){(V2){0, 0}, (V2){10000, 10000}};
  maze->tree.used = 0;
  maze->width = 0;
  maze->height = 0;

  uint32_t x = 0;
  uint32_t y = 0;

  int fd = open(filename, O_RDONLY);
  if (fd == -1)
  {
    printf("Failed to open file.\n");
    exit(1);
  }
  
  struct stat sb;
  if (fstat(fd, &sb) == -1)
  {
    printf("Failed to open file.\n");
    exit(1);
  }

  char * file = (char * )mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (file == MAP_FAILED)
  {
    printf("Failed to open file.\n");
    exit(1);
  }

  char cell_str[3] = {};
  char * f_ptr = file;
  while (f_ptr < (file + sb.st_size))
  {
    cell_str[0] = f_ptr[0];
    cell_str[1] = f_ptr[1];
    
    Cell new_cell = {};
    new_cell.type = CELL_NULL;

    if ((cell_str[0] == '^') &&
        (cell_str[1] == '^'))
    {
      new_cell.type = CELL_START;
    }
    else if ((cell_str[0] == '.') &&
             (cell_str[1] == '.'))
    {
      new_cell.type = CELL_PATH;
    }
    else if (((cell_str[0] == '#') && (cell_str[1] == '#')) ||
             ((cell_str[0] == '`') && (cell_str[1] == '`')))
    {
      new_cell.type = CELL_WALL;
    }
    else if ((cell_str[0] == '(') && (cell_str[1] == ')'))
    {
      new_cell.type = CELL_HOLE;
    }
    else if ((cell_str[0] == '<') && (cell_str[1] == '>'))
    {
      new_cell.type = CELL_SPLITTER;
    }
    else if (isLetter(cell_str[0]) && (isLetter(cell_str[1]) || isNum(cell_str[1])))
    {
      // uint32_t function_index = get_function_index(cell_str);
      // uint32_t function_parse_bytes_read = 0;
      // if ((sscanf((cell_str + offset + bytes_read), " -> %n", &function_parse_bytes_read) != EOF) && (function_parse_bytes_read > 0))
      // {
      //   Function * function = maze->functions + function_index;
      //   function->type = FunctionAssignment;
      //   function->name[0] = cell_str[0];
      //   function->name[1] = cell_str[1];

      //   bytes_read += function_parse_bytes_read;
      // }
      // else
      // {
      new_cell.type = CELL_FUNCTION;
      //   new_cell.data.function_index = function_index;
      // }
    }
    else if ((cell_str[0] == '-') && (cell_str[1] == '-'))
    {
      new_cell.type = CELL_ONCE;
    }
    else if ((cell_str[0] == '*') && (cell_str[1] == '*'))
    {
      new_cell.type = CELL_SIGNAL;
    }
    else if ((cell_str[0] == '>') && (cell_str[1] == '>'))
    {
      new_cell.type = CELL_INC;
    }
    else if ((cell_str[0] == '<') && (cell_str[1] == '<'))
    {
      new_cell.type = CELL_DEC;
    }
    else if ((cell_str[0] == '%') && ((cell_str[1] == 'U') ||
                                      (cell_str[1] == 'u')))
    {
      new_cell.type = CELL_UP;
    }
    else if ((cell_str[0] == '%') && ((cell_str[1] == 'D') ||
                                      (cell_str[1] == 'd')))
    {
      new_cell.type = CELL_DOWN;
    }
    else if ((cell_str[0] == '%') && ((cell_str[1] == 'L') ||
                                      (cell_str[1] == 'l')))
    {
      new_cell.type = CELL_LEFT;
    }
    else if ((cell_str[0] == '%') && ((cell_str[1] == 'R') ||
                                      (cell_str[1] == 'r')))
    {
      new_cell.type = CELL_RIGHT;
    }
    else if (isNum(cell_str[0]) && isNum(cell_str[1]))
    {
      new_cell.type = CELL_PAUSE;

      uint32_t digit0 = cell_str[0] - '0';
      uint32_t digit1 = cell_str[1] - '0';
      new_cell.data.pause = (10 * digit0) + digit1;
    }

    if (new_cell.type == CELL_NULL)
    {
      if (cell_str[0] == '\n')
      {
        if (x > maze->width)
        {
          maze->width = x;
        }
        x = 0;
        ++y;
        // printf("\n");
      }
      f_ptr += 1;
    }
    else
    {
      Cell * cell = get_cell(maze, x, y, game_memory);
      cell->type = new_cell.type;
      cell->data = new_cell.data;

      f_ptr += 2;
      ++x;

      // printf("%s ", cell_str);
    }
  }
  maze->height = y;

  printf("(%d, %d)\n", maze->width, maze->height);

  return maze;
}