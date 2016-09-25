#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define consume_whitespace(ptr, end) while ((*(ptr) == ' ' || *(ptr) == '\t') && (ptr) < (end)) ++(ptr)
#define consume_whitespace_nl(ptr, end) while ((*(ptr) == ' ' || *(ptr) == '\t' || isNewline(*(ptr))) && (ptr) < (end)) ++(ptr)
#define consume_while(ptr, func, end) while (func(*(ptr)) && (ptr) < (end)) ++(ptr)
#define consume_until(ptr, func, end) while (!func(*(ptr)) && (ptr) < (end)) ++(ptr)
#define consume_until_newline(ptr, end) consume_until(ptr, isNewline, end)


bool
str_eq(const char *a, const char *b, u32 n)
{
  for (u32 i = 0; i < n; ++i)
  {
    if (a[i] != b[i])
    {
      return false;;
    }
  }

  return true;
}


b32
isNum(char character)
{
  b32 result = (character >= '0') && (character <= '9');
  return result;
}


b32
isLetter(char character)
{
  b32 result = (((character >= 'a') && (character <= 'z')) ||
                ((character >= 'A') && (character <= 'Z')));
  return result;
}


b32
isNewline(char character)
{
  b32 result = (character == '\n') || (character == '\r');
  return result;
}


char *
get_num(char *ptr, char *f_end, u32 *result)
{
  char *num_start = ptr;

  consume_while(ptr, isNum, f_end);

  *result = 0;
  u32 num_length = ptr - num_start;

  for (u32 num_pos = 0;
       num_pos < num_length;
       ++num_pos)
  {
    u32 digit = *(num_start + num_pos) - '0';
    *result += digit * pow(10, (num_length - num_pos - 1));
  }

  return ptr;
}


char *
get_direction(char *ptr, V2 *result)
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
        ptr += 2;
      } break;
      case 'D':
      case 'd':
      {
        *result = DOWN;
        ptr += 2;
      } break;
      case 'L':
      case 'l':
      {
        *result = LEFT;
        ptr += 2;
      } break;
      case 'R':
      case 'r':
      {
        *result = RIGHT;
        ptr += 2;
      } break;
    }
  }

  return ptr;
}


char *
parse_function_definition(Function functions[], char cell_str[2], char *f_ptr, char *f_end, Cell *cell)
{
  u32 function_index = get_function_index(cell_str);

  consume_whitespace(f_ptr, f_end);

  if (str_eq(f_ptr, "->", 2))
  {
    // Function Definition
    log(L_Parser, "Parsing function definition,");

    f_ptr += 2;

    Function *function = functions + function_index;

    if (function->type != FUNCTION_NULL)
    {
      log(L_Parser, "Function already defined, ignore this definition.");
    }
    else
    {
      function->name[0] = cell_str[0];
      function->name[1] = cell_str[1];

      consume_whitespace(f_ptr, f_end);

      if (*f_ptr == '=')
      {
        log(L_Parser, "  Type: Assignment,");

        ++f_ptr;
        consume_whitespace(f_ptr, f_end);

        u32 assignment_value;
        char *end_num_f_ptr = get_num(f_ptr, f_end, &assignment_value);

        if (end_num_f_ptr == f_ptr)
        {
          log(L_Parser, "  Function invalid: Assignment value missing.");
        }
        else
        {
          log(L_Parser, "  Value: %d", assignment_value);
          f_ptr = end_num_f_ptr;

          function->value = assignment_value;
          function->type = FUNCTION_ASSIGNMENT;
        }
      }
      else if ((f_ptr[0] == '+' ||
                f_ptr[0] == '-' ||
                f_ptr[0] == '*' ||
                f_ptr[0] == '/') && f_ptr[1] == '=')
      {
        log(L_Parser, "  Type: Operator,");

        char sign = f_ptr[0];

        f_ptr += 2;
        consume_whitespace(f_ptr, f_end);

        u32 assignment_value;
        char *end_num_f_ptr = get_num(f_ptr, f_end, &assignment_value);
        if (end_num_f_ptr == f_ptr)
        {
          log(L_Parser, "  Function invalid: Operator value missing.");
        }
        else
        {
          log(L_Parser, "  Value: %d", assignment_value);
          f_ptr = end_num_f_ptr;

          function->value = assignment_value;

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
        }
      }
      else if (str_eq(f_ptr, "IF", 2) || str_eq(f_ptr, "if", 2))
      {
        log(L_Parser, "  Type: Conditional,");

        f_ptr += 2;
        consume_whitespace(f_ptr, f_end);

        FunctionType conditional_func_type;
        b32 valid_condition = true;

        if (str_eq(f_ptr, "<=", 2))
        {
          f_ptr += 2;
          conditional_func_type = FUNCTION_LESS_EQUAL;
        }
        if (*f_ptr == '<')
        {
          f_ptr += 1;
          conditional_func_type = FUNCTION_LESS;
        }
        else if (str_eq(f_ptr, ">=", 2))
        {
          f_ptr += 2;
          conditional_func_type = FUNCTION_GREATER_EQUAL;
        }
        else if (*f_ptr == '>')
        {
          f_ptr += 1;
          conditional_func_type = FUNCTION_GREATER;
        }
        else if (str_eq(f_ptr, "==", 2))
        {
          f_ptr += 2;
          conditional_func_type = FUNCTION_EQUAL;
        }
        else if (str_eq(f_ptr, "!=", 2))
        {
          f_ptr += 2;
          conditional_func_type = FUNCTION_NOT_EQUAL;
        }
        else
        {
          log(L_Parser, "  Invalid function: Conditional operator missing.");
          valid_condition = false;
        }

        if (valid_condition)
        {
          consume_whitespace(f_ptr, f_end);

          u32 condition_value;
          char *end_num_f_ptr = get_num(f_ptr, f_end, &condition_value);
          if (end_num_f_ptr == f_ptr)
          {
            log(L_Parser, "  Invalid function: Missing condition value.");
          }
          else
          {
            log(L_Parser, "  Condition value: %d", condition_value);
            f_ptr = end_num_f_ptr;

            consume_whitespace(f_ptr, f_end);

            if (!(str_eq(f_ptr, "THEN", 4) || str_eq(f_ptr, "then", 4)))
            {
              log(L_Parser, "  Invalid function: Missing 'THEN' keyword.");
            }
            else
            {
              f_ptr += 4;
              consume_whitespace(f_ptr, f_end);

              V2 true_direction;
              char *end_true_direction_f_ptr = get_direction(f_ptr, &true_direction);
              if (end_true_direction_f_ptr == f_ptr)
              {
                log(L_Parser, "  Invalid function: Missing 'THEN' direction.");
              }
              else
              {
                log(L_Parser, "  THEN direction: (%f, %f)", true_direction.x, true_direction.y);
                f_ptr = end_true_direction_f_ptr;

                consume_whitespace(f_ptr, f_end);

                b32 valid_conditional_function = true;
                b32 else_exists = false;
                V2 false_direction;

                // ELSE is optional
                if (str_eq(f_ptr, "ELSE", 4) || str_eq(f_ptr, "else", 4))
                {
                  else_exists = true;

                  f_ptr += 4;
                  consume_whitespace(f_ptr, f_end);

                  char *end_false_direction_f_ptr = get_direction(f_ptr, &false_direction);
                  if (end_false_direction_f_ptr == f_ptr)
                  {
                    log(L_Parser, "Invalid function: Missing 'ELSE' direction.");
                    valid_conditional_function = false;
                  }
                  else
                  {
                    log(L_Parser, "  ELSE direction: (%f, %f)", false_direction.x, false_direction.y);
                    f_ptr = end_false_direction_f_ptr;

                    valid_conditional_function = true;
                  }
                }

                if (valid_conditional_function)
                {
                  function->type = conditional_func_type;
                  function->conditional.value = condition_value;
                  function->conditional.true_direction = true_direction;

                  function->conditional.else_exists = else_exists;
                  if (else_exists)
                  {
                    function->conditional.false_direction = false_direction;
                  }
                }

              }
            }
          }
        }
      }
    }

    consume_until_newline(f_ptr, f_end);
  }

  return f_ptr;
}


char *
parse_cell(Maze *maze, char cell_str[2], char *f_ptr, char *f_end, Cell *cell)
{

  if (str_eq(cell_str, "^^", 2))
  {
    cell->type = CELL_START;
  }
  else if (str_eq(cell_str, "..", 2))
  {
    cell->type = CELL_PATH;
  }
  else if (str_eq(cell_str, "##", 2) ||
           str_eq(cell_str, "  ", 2) ||
           str_eq(cell_str, "``", 2))
  {
    cell->type = CELL_WALL;
  }
  else if (str_eq(cell_str, "()", 2))
  {
    cell->type = CELL_HOLE;
  }
  else if (str_eq(cell_str, "<>", 2))
  {
    cell->type = CELL_SPLITTER;
  }
  else if (isLetter(cell_str[0]) && (isLetter(cell_str[1]) || isNum(cell_str[1])))
  {
    char *end_function_name_f_ptr = f_ptr + 2;
    char *end_function_definition_f_ptr = parse_function_definition(maze->functions, cell_str, end_function_name_f_ptr, f_end, cell);
    if (end_function_definition_f_ptr != end_function_name_f_ptr)
    {
      f_ptr = end_function_definition_f_ptr;
    }
    else
    {
      cell->type = CELL_FUNCTION;
      cell->function_index = get_function_index(cell_str);
    }

  }
  else if (str_eq(cell_str, "--", 2))
  {
    cell->type = CELL_ONCE;
  }
  else if ((cell_str[0] == '*') && ((cell_str[1] == 'U') ||
                                    (cell_str[1] == 'u')))
  {
    cell->type = CELL_UP_UNLESS_DETECT;
  }
  else if ((cell_str[0] == '*') && ((cell_str[1] == 'D') ||
                                    (cell_str[1] == 'd')))
  {
    cell->type = CELL_DOWN_UNLESS_DETECT;
  }
  else if ((cell_str[0] == '*') && ((cell_str[1] == 'L') ||
                                    (cell_str[1] == 'l')))
  {
    cell->type = CELL_LEFT_UNLESS_DETECT;
  }
  else if ((cell_str[0] == '*') && ((cell_str[1] == 'R') ||
                                    (cell_str[1] == 'r')))
  {
    cell->type = CELL_RIGHT_UNLESS_DETECT;
  }
  else if (str_eq(cell_str, ">>", 2))
  {
    cell->type = CELL_INC;
  }
  else if (str_eq(cell_str, "<<", 2))
  {
    cell->type = CELL_DEC;
  }
  else if ((cell_str[0] == '%') && ((cell_str[1] == 'U') ||
                                    (cell_str[1] == 'u')))
  {
    cell->type = CELL_UP;
  }
  else if ((cell_str[0] == '%') && ((cell_str[1] == 'D') ||
                                    (cell_str[1] == 'd')))
  {
    cell->type = CELL_DOWN;
  }
  else if ((cell_str[0] == '%') && ((cell_str[1] == 'L') ||
                                    (cell_str[1] == 'l')))
  {
    cell->type = CELL_LEFT;
  }
  else if ((cell_str[0] == '%') && ((cell_str[1] == 'R') ||
                                    (cell_str[1] == 'r')))
  {
    cell->type = CELL_RIGHT;
  }
  else if (isNum(cell_str[0]) && isNum(cell_str[1]))
  {
    cell->type = CELL_PAUSE;

    u32 digit0 = cell_str[0] - '0';
    u32 digit1 = cell_str[1] - '0';
    cell->pause = (10 * digit0) + digit1;
  }

  if (cell->type != CELL_NULL)
  {
    f_ptr += 2;
  }

  return f_ptr;
}


// TODO: Parse comments!

void
parse(Maze *maze, Memory *memory, const char *filename)
{
  maze->tree.bounds = (Rectangle){(V2){0, 0}, (V2){10000, 10000}};
  maze->tree.used = 0;
  maze->width = 0;
  maze->height = 0;

  u32 x = 0;
  u32 y = 0;

  u32 fd = open(filename, O_RDONLY);
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

  char *file = (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (file == MAP_FAILED)
  {
    printf("Failed to open file.\n");
    exit(1);
  }

  char cell_str[2] = {};
  char *f_ptr = file;
  char *f_end = f_ptr + sb.st_size;
  while (f_ptr < f_end)
  {
    cell_str[0] = f_ptr[0];
    cell_str[1] = f_ptr[1];

    Cell new_cell = {};
    new_cell.type = CELL_NULL;
    f_ptr = parse_cell(maze, cell_str, f_ptr, f_end, &new_cell);

    if (new_cell.type != CELL_NULL)
    {
      Cell *cell = get_cell(maze, x, y, memory);
      cell->type = new_cell.type;
      cell->pause = new_cell.pause;
      cell->function_index = new_cell.function_index;

      cell->name[0] = cell_str[0];
      cell->name[1] = cell_str[1];

      log_s(L_Parser, "%.2s ", cell_str);
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
        log_s(L_Parser, "\n");
      }
      f_ptr += 1;
    }
  }
  maze->height = y;

  log_s(L_Parser, "\n");
  log(L_Parser, "(%d, %d)", maze->width, maze->height);
}