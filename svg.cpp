struct ParsePathCommandResult
{
  V2 last_point;
  char *c;
  b32 found_command;
  LineSegment *added_segment;
};

ParsePathCommandResult
parse_path_command(Memory *memory, char command, char *c, char *c_after_command, char *end_f, SVGPath *path, V2 last_point)
{
  ParsePathCommandResult result;

  b32 delta = false;
  b32 found_command = true;
  LineSegment *line_seg = 0;

  switch (command)
  {
    case ('m'):
      delta = true;
    case ('M'):
    {
      // Move
      c = c_after_command;

      V2 d_move;
      consume_until(c, is_num_or_sign, end_f);
      c = get_num(c, end_f, &d_move.x);
      consume_until(c, is_num_or_sign, end_f);
      c = get_num(c, end_f, &d_move.y);

      if (delta)
      {
        last_point += d_move;
      }
      else
      {
        last_point = d_move;
      }
    } break;

    case ('l'):
      delta = true;
    case ('L'):
    {
      // Line
      c = c_after_command;

      ++path->n_segments;
      line_seg = push_struct(memory, LineSegment);
      line_seg->start = last_point;

      consume_until(c, is_num_or_sign, end_f);
      c = get_num(c, end_f, &line_seg->end.x);
      consume_until(c, is_num_or_sign, end_f);
      c = get_num(c, end_f, &line_seg->end.y);

      if (delta)
      {
        line_seg->end += line_seg->start;
      }
    } break;

    case ('h'):
      delta = true;
    case ('H'):
    {
      // Horizontal line
      c = c_after_command;

      ++path->n_segments;
      line_seg = push_struct(memory, LineSegment);
      line_seg->start = last_point;
      line_seg->end.y = last_point.y;

      consume_until(c, is_num_or_sign, end_f);
      c = get_num(c, end_f, &line_seg->end.x);

      if (delta)
      {
        line_seg->end.x += line_seg->start.x;
      }
    } break;

    case ('v'):
      delta = true;
    case ('V'):
    {
      // Vertical line
      c = c_after_command;

      ++path->n_segments;
      line_seg = push_struct(memory, LineSegment);
      line_seg->start = last_point;
      line_seg->end.x = last_point.x;

      consume_until(c, is_num_or_sign, end_f);
      c = get_num(c, end_f, &line_seg->end.y);

      if (delta)
      {
        line_seg->end.y += line_seg->start.y;
      }
    } break;

    case ('Z'):
    case ('z'):
    {
      // Close path
      c = c_after_command;

      ++path->n_segments;
      line_seg = push_struct(memory, LineSegment);
      line_seg->start = last_point;
      line_seg->start = path->segments[0].start;
    } break;

    default:
    {
      found_command = false;
    } break;
  }

  if (line_seg != 0)
  {
    result.last_point = line_seg->end;
  }
  else
  {
    result.last_point = last_point;
  }

  result.c = c;
  result.found_command = found_command;
  result.added_segment = line_seg;

  return result;
}


SVGPath
parse_path(Memory *memory, XMLTag *path_tag)
{
  SVGPath path = {.segments = 0, .n_segments = 0};

  String d_attr = get_attr_value(path_tag, String("d"));

  log(L_SVG, "Parsing: '%.*s'", d_attr.length, d_attr.text);

  if (d_attr.text != 0)
  {
    char *end_f = d_attr.text + d_attr.length;

    V2 current_point = (V2){0, 0};

    char last_command = 0;

    char *c = d_attr.text;
    while (c < end_f)
    {
      consume_whitespace(c, end_f);

      char command = *c;
      char *after_command = c + 1;

      ParsePathCommandResult command_parse_result = parse_path_command(memory, command, c, after_command, end_f, &path, current_point);
      current_point = command_parse_result.last_point;
      c = command_parse_result.c;
      if (command_parse_result.added_segment && path.segments == 0)
      {
        path.segments = command_parse_result.added_segment;
      }

      if (command_parse_result.found_command)
      {
        last_command = command;
      }
      else
      {
        if (last_command != 0)
        {
          // Special case where subsequent coords after a move command, are treated as line commands
          if (last_command == 'm')
          {
            last_command = 'l';
          }
          if (last_command == 'M')
          {
            last_command = 'L';
          }

          command_parse_result = parse_path_command(memory, last_command, c, c, end_f, &path, current_point);
          current_point = command_parse_result.last_point;
          c = command_parse_result.c;
          if (command_parse_result.added_segment && path.segments == 0)
          {
            path.segments = command_parse_result.added_segment;
          }

          assert(command_parse_result.found_command);
        }
        else
        {
          printf("Error: SVG path doesn't contain a command\n");
        }
      }
    }
  }

  return path;
}


void
get_svg_operations(Memory *memory, XMLTag *tag, SVGOperation **result)
{
  XMLTag *child = tag->first_child;
  while (child)
  {
    if (str_eq(child->name, String("path")))
    {
      SVGOperation *old_start = *result;
      *result = push_struct(memory, SVGOperation);

      (*result)->type = SVG_OP_PATH;
      (*result)->next = old_start;

      (*result)->path = parse_path(memory, child);
    }
    else if (str_eq(child->name, String("g")))
    {
      // TODO: Handle transform attribute!

      get_svg_operations(memory, child, result);
    }
    else if (str_eq(child->name, String("svg")))
    {
      get_svg_operations(memory, child, result);
    }

    child = child->next_sibling;
  }
}


void
test_log_svg_operations(SVGOperation *svg_operations)
{
  SVGOperation *current = svg_operations;
  while (current)
  {
    if (current->path.segments)
    {
      LineSegment *line_segments = current->path.segments;

      for (u32 i = 0; i < current->path.n_segments; ++i)
      {
        LineSegment *line_seg = line_segments + i;
        // log(L_SVG, "  segment:");
        // log(L_SVG, "    (%f,%f)", line_seg->start.x, line_seg->start.y);
        // log(L_SVG, "    (%f,%f)", line_seg->end.x, line_seg->end.y);
      }
    }
    else
    {
      log(L_SVG, "ERROR");
    }

    current = current->next;
  }
}