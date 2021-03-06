const u32 CELL_LENGTH = 3;
const u32 LINE_BREAK_LENGTH = 1;


void
find_min_max(QuadTree *tree, vec2 *most_left, vec2 *most_top, vec2 *most_right, vec2 *most_bottom)
{
  if (tree)
  {
    for (u32 cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell *cell = tree->cells + cell_index;

      if (cell->type != CELL_NULL)
      {
        if (cell->x < most_left->x)
        {
          *most_left = Vec2(cell->x, cell->y);
        }
        if (cell->y < most_top->y)
        {
          *most_top = Vec2(cell->x, cell->y);
        }
        if (cell->x > most_right->x)
        {
          *most_right = Vec2(cell->x, cell->y);
        }
        if (cell->y > most_bottom->y)
        {
          *most_bottom = Vec2(cell->x, cell->y);
        }
      }
    }

    find_min_max(tree->top_left, most_left, most_top, most_right, most_bottom);
    find_min_max(tree->top_right, most_left, most_top, most_right, most_bottom);
    find_min_max(tree->bottom_left, most_left, most_top, most_right, most_bottom);
    find_min_max(tree->bottom_right, most_left, most_top, most_right, most_bottom);
  }
}


void
write_cells(QuadTree *tree, u8 *file, u32 width, u32 height, vec2 maze_origin, Functions *functions)
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
      u8 *file_pos = file + cell_file_offset;

      if (cell->type == CELL_FUNCTION)
      {
        Function *function = functions->hash_table + cell->function_index;
        memcpy(file_pos, function->name, 2);
      }
      else if (cell->type == CELL_PAUSE)
      {
        formatted_string(file_pos, 3, u8("%02d"), cell->pause);
      }
      else if (cell->type != CELL_NULL)
      {
        const u8 *text = CELL_TYPE_TXT[cell->type];
        memcpy(file_pos, text, 2);
      }

      file_pos[2] = ' ';
    }

    write_cells(tree->top_left, file, width, height, maze_origin, functions);
    write_cells(tree->top_right, file, width, height, maze_origin, functions);
    write_cells(tree->bottom_left, file, width, height, maze_origin, functions);
    write_cells(tree->bottom_right, file, width, height, maze_origin, functions);
  }
}


vec2
get_maze_size(Maze *maze)
{
  vec2 most_left   = Vec2(maze->tree.cells[maze->tree.used-1].x, maze->tree.cells[maze->tree.used-1].y);
  vec2 most_top    = most_left;
  vec2 most_right  = most_left;
  vec2 most_bottom = most_left;

  find_min_max(&maze->tree, &most_left, &most_top, &most_right, &most_bottom);

  vec2 result = Vec2((u32)(most_right - most_left).x + 1,
                     (u32)(most_bottom - most_top).y + 1);

  return result;
}


const u32 MAX_FUNC_LENGTH = 40;


u32
serialize_function(u8 buffer[MAX_FUNC_LENGTH], Function *function)
{
  u32 pos = 0;
  s32 space_left = MAX_FUNC_LENGTH;

  #define INC_POS(n) ( pos += (n), space_left -= (n), assert(space_left > 0) )

  memcpy(buffer+pos, function->name, 2);  INC_POS(2);
  memcpy(buffer+pos, " -> ", 4);  INC_POS(4);

  u8 symbol[2];
  u32 n_written;
  switch (function->type)
  {
    case (FUNCTION_ASSIGNMENT):    copy_string(symbol, u8(""), 2);    goto LBL_FUNCTION_ASSIGN;
    case (FUNCTION_INCREMENT):     copy_string(symbol, u8("+"), 2);   goto LBL_FUNCTION_ASSIGN;
    case (FUNCTION_DECREMENT):     copy_string(symbol, u8("-"), 2);   goto LBL_FUNCTION_ASSIGN;
    case (FUNCTION_MULTIPLY):      copy_string(symbol, u8("*"), 2);   goto LBL_FUNCTION_ASSIGN;
    case (FUNCTION_DIVIDE):        copy_string(symbol, u8("/"), 2);   goto LBL_FUNCTION_ASSIGN;

    LBL_FUNCTION_ASSIGN: {
      log(L_Serializer, u8("Function type: Assign"));
      n_written = formatted_string(buffer+pos, space_left, u8("%.1s= %d"), symbol, function->value);
      INC_POS(n_written);
    } break;

    case (FUNCTION_LESS):          copy_string(symbol, u8("<"), 2);   goto LBL_FUNCTION_COND;
    case (FUNCTION_LESS_EQUAL):    copy_string(symbol, u8("<="), 2);  goto LBL_FUNCTION_COND;
    case (FUNCTION_EQUAL):         copy_string(symbol, u8("=="), 2);  goto LBL_FUNCTION_COND;
    case (FUNCTION_NOT_EQUAL):     copy_string(symbol, u8("!="), 2);  goto LBL_FUNCTION_COND;
    case (FUNCTION_GREATER_EQUAL): copy_string(symbol, u8(">="), 2);  goto LBL_FUNCTION_COND;
    case (FUNCTION_GREATER):       copy_string(symbol, u8(">"), 2);   goto LBL_FUNCTION_COND;

    LBL_FUNCTION_COND: {
      log(L_Serializer, u8("Function type: Cond"));
      u8 true_direction = vector_to_compass_dir(function->conditional.true_direction);
      n_written = formatted_string(buffer+pos, space_left, u8("IF %.2s %d THEN %%%c"), symbol, function->conditional.value, true_direction);
      INC_POS(n_written);

      if (function->conditional.else_exists)
      {
        log(L_Serializer, u8("  with else"));
        u8 false_direction = vector_to_compass_dir(function->conditional.false_direction);
        n_written = formatted_string(buffer+pos, space_left, u8(" ELSE %%%c"), false_direction);
        INC_POS(n_written);
      }

    } break;

    default:
    {
      invalid_code_path;
    }
  }

  buffer[pos] = '\n';  INC_POS(1);

  return pos;
}


void
serialize_maze(Maze *maze, Functions *functions, const u8 *filename)
{
  // TMP: For testing without overwriting
#if 0
  ((u8 *)filename)[0] = '_';
#endif

  vec2 most_left   = Vec2(maze->tree.cells[maze->tree.used-1].x,
                          maze->tree.cells[maze->tree.used-1].y);
  vec2 most_top    = most_left;
  vec2 most_right  = most_left;
  vec2 most_bottom = most_left;

  find_min_max(&maze->tree, &most_left, &most_top, &most_right, &most_bottom);

  u32 width  = (u32)(most_right - most_left).x + 1;
  u32 height = (u32)(most_bottom - most_top).y + 1;

  u32 n_funcs = 8;

  u32 maze_length = CELL_LENGTH * width * height +
                    LINE_BREAK_LENGTH * height;
  u32 gap_length = 1;
  u32 functions_length = MAX_FUNC_LENGTH * n_funcs;

  u32 length = maze_length + gap_length + functions_length;

  File file;
  open_file(filename, &file, true, length);

  log(L_Serializer, u8("Serializing maze"));

  memset(file.write, ' ', length);
  write_cells(&maze->tree, file.write, width, height, (vec2){most_left.x, most_top.y}, functions);

  // Add line breaks to maze
  for (u32 line = 0;
       line < height;
       ++line)
  {
    u32 file_offset = (line+1) * width * CELL_LENGTH + line;
    file.write[file_offset] = '\n';
  }

  // Add line breaks for gap between maze and function defs
  for (u32 line = 0;
       line < gap_length;
       ++line)
  {
    u32 file_offset = maze_length + line;
    file.write[file_offset] = '\n';
  }

  log(L_Serializer, u8("Serializing functions"));

  u8 tmp_function_buffer[MAX_FUNC_LENGTH];
  u32 length_funcs_written = 0;
  u32 functions_serialized = 0;

  for (u32 func_index = 0;
       func_index < MAX_FUNCTIONS;
       ++func_index)
  {
    Function *function = functions->hash_table + func_index;

    if (function->type != FUNCTION_NULL)
    {
      log(L_Serializer, u8("Serializing function"));

      ++functions_serialized;
      if (functions_serialized > n_funcs)
      {
        log(L_Serializer, u8("Error: Found more functions in hash table than told, ignoring extra functions."));
        break;
      }
      else
      {
        u32 file_pos = maze_length + gap_length + length_funcs_written;

        u32 func_length = serialize_function(tmp_function_buffer, function);
        memcpy(file.write + file_pos, tmp_function_buffer, func_length);

        log(L_Serializer, u8("%.*s"), func_length, tmp_function_buffer);
        length_funcs_written += func_length;
      }
    }
  }

  u32 length_written = maze_length + gap_length + length_funcs_written;
  log(L_Serializer, u8("Overestimated bytes: %d"), length - length_written);

  close_file(&file, length_written);
}

