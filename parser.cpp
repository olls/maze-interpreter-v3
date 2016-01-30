#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define consume_whitespace(ptr, end) while ((*(ptr) == ' ' || *(ptr) == '\t') && (ptr) < end) ++(ptr)
#define consume_whitespace_nl(ptr, end) while ((*(ptr) == ' ' || *(ptr) == '\t' || isNewline(*(ptr))) && (ptr) < end) ++(ptr)
#define consume_until_newline(ptr, end) while (!isNewline(*(ptr)) && (ptr) < end) ++(ptr)
#define consume_while(ptr, func, end) while (func(*(ptr)) && (ptr) < end) ++(ptr)


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


bool
isNewline(char character)
{
  bool result = (character == '\n') || (character == '\r');
  return result;
}


char *
get_num(char * ptr, char * f_end, uint32_t * result)
{
  char * num_start = ptr;

  consume_while(ptr, isNum, f_end);

  *result = 0;
  uint32_t num_length = ptr - num_start;

  for (uint32_t num_pos = 0;
       num_pos < num_length;
       ++num_pos)
  {
    uint32_t digit = *(num_start + num_pos) - '0';
    *result += digit * pow(10, (num_length - num_pos - 1));
  }

  return ptr;
}


char *
get_direction(char * ptr, enum Direction * result)
{
  *result = STATIONARY;

  if (ptr[0] == '%')
  {
    switch (ptr[1])
    {
      case 'U':
      case 'u':
      {
        *result = UP;
      } break;
      case 'D':
      case 'd':
      {
        *result = DOWN;
      } break;
      case 'L':
      case 'l':
      {
        *result = LEFT;
      } break;
      case 'R':
      case 'r':
      {
        *result = RIGHT;
      } break;
      default:
      {
        return ptr;
      }
    }
    ptr += 2;
  }

  return ptr;
}


Maze *
parse(GameMemory * game_memory, const char * filename)
{
  Maze * maze = take_struct_mem(game_memory, Maze, 1);
  maze->tree.bounds = (Rectangle){(V2){0, 0}, (V2){10000, 10000}};
  maze->tree.used = 0;
  maze->width = 0;
  maze->height = 0;

  uint32_t x = 0;
  uint32_t y = 0;

  uint32_t fd = open(filename, O_RDONLY);
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

  char * file = (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (file == MAP_FAILED)
  {
    printf("Failed to open file.\n");
    exit(1);
  }

  char cell_str[3] = {};
  char * f_ptr = file;
  char * f_end = f_ptr + sb.st_size;
  while (f_ptr < f_end)
  {
    cell_str[0] = f_ptr[0];
    cell_str[1] = f_ptr[1];

    Cell new_cell = {};
    new_cell.type = CELL_NULL;

    if (strncmp(cell_str, "^^", 2) == 0)
    {
      new_cell.type = CELL_START;
    }
    else if (strncmp(cell_str, "..", 2) == 0)
    {
      new_cell.type = CELL_PATH;
    }
    else if (strncmp(cell_str, "##", 2) == 0 ||
             strncmp(cell_str, "  ", 2) == 0 ||
             strncmp(cell_str, "``", 2) == 0)
    {
      new_cell.type = CELL_WALL;
    }
    else if (strncmp(cell_str, "()", 2) == 0)
    {
      new_cell.type = CELL_HOLE;
    }
    else if (strncmp(cell_str, "<>", 2) == 0)
    {
      new_cell.type = CELL_SPLITTER;
    }
    else if (isLetter(cell_str[0]) && (isLetter(cell_str[1]) || isNum(cell_str[1])))
    {
      uint32_t function_index = get_function_index(cell_str);
      uint32_t function_parse_bytes_read = 0;

      char * ptr = f_ptr + 2;
      consume_whitespace(ptr, f_end);

      if (ptr[0] == '-' &&
          ptr[1] == '>')
      {
        // Function Definition
        ptr += 2;
        Function * function = maze->functions + function_index;
        function->name[0] = cell_str[0];
        function->name[1] = cell_str[1];

        consume_whitespace(ptr, f_end);

        if (*ptr == '=')
        {
          consume_whitespace(ptr, f_end);

          ptr = get_num(ptr, f_end, &(function->value));
          function->type = FUNCTION_ASSIGNMENT;

          consume_until_newline(ptr, f_end);
          f_ptr = ptr;

        }
        else if (*ptr == '+' ||
                 *ptr == '-' ||
                 *ptr == '*' ||
                 *ptr == '/')
        {
          char sign = *ptr++;
          if (*ptr++ == '=')
          {
            switch (sign)
            {
              case '+':
              {
                function->type = FUNCTION_INCREMENT;
              } break;
              case '-':
              {
                function->type = FUNCTION_DECREMENT;
              } break;
              case '*':
              {
                function->type = FUNCTION_MULTIPLY;
              } break;
              case '/':
              {
                function->type = FUNCTION_DIVIDE;
              } break;
            }
            consume_whitespace(ptr, f_end);

            ptr = get_num(ptr, f_end, &(function->value));

            consume_until_newline(ptr, f_end);
            f_ptr = ptr;
          }

        }
        else if (strncmp(ptr, "IF", 2) == 0)
        {
          ptr += 2;
          consume_whitespace(ptr, f_end);

          FunctionType conditional_func_type;
          bool valid_condition = true;
          bool value_required = true;
          if (strncmp(ptr, "<=", 2) == 0)
          {
            ptr += 2;
            conditional_func_type = FUNCTION_LESS_EQUAL;
          }
          if (*ptr == '<')
          {
            ptr += 1;
            conditional_func_type = FUNCTION_LESS;
          }
          else if (strncmp(ptr, ">=", 2) == 0)
          {
            ptr += 2;
            conditional_func_type = FUNCTION_GREATER_EQUAL;
          }
          else if (*ptr == '>')
          {
            ptr += 1;
            conditional_func_type = FUNCTION_GREATER;
          }
          else if (strncmp(ptr, "==", 2) == 0)
          {
            ptr += 2;
            conditional_func_type = FUNCTION_EQUAL;
          }
          else if (strncmp(ptr, "!=", 2) == 0)
          {
            ptr += 2;
            conditional_func_type = FUNCTION_NOT_EQUAL;
          }
          else if (strncmp(ptr, "**", 2) == 0)
          {
            ptr += 2;
            conditional_func_type = FUNCTION_SIGNAL;
            value_required = false;
          }
          else
          {
            printf("Conditional \n");
            valid_condition = false;
          }

          uint32_t condition_value;
          if (value_required && valid_condition)
          {
            consume_whitespace(ptr, f_end);

            char * num_start = ptr;
            ptr = get_num(ptr, f_end, &(condition_value));
            if (num_start == ptr)
            {
              valid_condition = false;
            }
          }

          if (valid_condition)
          {
            consume_whitespace(ptr, f_end);

            if (strncmp(ptr, "THEN", 4) == 0)
            {
              ptr += 4;
              consume_whitespace(ptr, f_end);

              enum Direction true_direction;
              ptr = get_direction(ptr, &true_direction);
              if (true_direction != STATIONARY)
              {
                consume_whitespace(ptr, f_end);

                if (strncmp(ptr, "ELSE", 4) == 0)
                {
                  ptr += 4;
                  consume_whitespace(ptr, f_end);

                  enum Direction false_direction;
                  ptr = get_direction(ptr, &false_direction);
                  if (false_direction != STATIONARY)
                  {
                    function->type = conditional_func_type;
                    function->conditional.condition_value = condition_value;
                    function->conditional.true_direction = true_direction;
                    function->conditional.false_direction = false_direction;

                    consume_until_newline(ptr, f_end);
                    f_ptr = ptr;
                  }
                }
                else
                {
                  consume_until_newline(ptr, f_end);
                  f_ptr = ptr;
                }
              }
            }
          }
        }
      }
      else
      {
        // Funciton Call
        new_cell.type = CELL_FUNCTION;
        new_cell.function_index = function_index;
      }
    }
    else if (strncmp(cell_str, "--", 2) == 0)
    {
      new_cell.type = CELL_ONCE;
    }
    else if (strncmp(cell_str, "**", 2) == 0)
    {
      new_cell.type = CELL_SIGNAL;
    }
    else if (strncmp(cell_str, ">>", 2) == 0)
    {
      new_cell.type = CELL_INC;
    }
    else if (strncmp(cell_str, "<<", 2) == 0)
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
      new_cell.pause = (10 * digit0) + digit1;
    }

    if (new_cell.type != CELL_NULL)
    {
      Cell * cell = get_cell(maze, x, y, game_memory);
      cell->type = new_cell.type;
      cell->pause = new_cell.pause;
      cell->function_index = new_cell.function_index;

      printf("%s ", cell_str);

      f_ptr += 2;
      ++x;
    }
    else
    {
      if (cell_str[0] == '\n')
      {
        if (x > maze->width)
        {
          maze->width = x;
        }
        x = 0;
        ++y;
        printf("\n");
      }
      f_ptr += 1;
    }
  }
  maze->height = y;

  printf("(%d, %d)\n", maze->width, maze->height);

  return maze;
}