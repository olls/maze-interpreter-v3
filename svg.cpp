struct ParsePathCommandResult
{
  V2 last_point;
  char *c;
  b32 found_command;
  LineSegment *added_segment;
};

ParsePathCommandResult
parse_path_command(Memory *memory, char command, char *c, char *c_after_command, char *end_f, SVGPath *path, V2 last_point, V2 origin)
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
        last_point = origin + d_move;
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
      else
      {
        line_seg->start += origin;
        line_seg->end += origin;
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
      else
      {
        line_seg->start += origin;
        line_seg->end += origin;
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
      else
      {
        line_seg->start += origin;
        line_seg->end += origin;
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
parse_path(Memory *memory, XMLTag *path_tag, V2 origin)
{
  SVGPath path = {.segments = 0, .n_segments = 0};

  String d_attr = get_attr_value(path_tag, String("d"));

  log(L_SVG, "Parsing: '%.*s'", d_attr.length, d_attr.text);

  if (d_attr.text != 0)
  {
    char *end_f = d_attr.text + d_attr.length;

    V2 current_point = origin;

    char last_command = 0;

    char *c = d_attr.text;
    while (c < end_f)
    {
      consume_whitespace(c, end_f);

      char command = *c;
      char *after_command = c + 1;

      ParsePathCommandResult command_parse_result = parse_path_command(memory, command, c, after_command, end_f, &path, current_point, origin);
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

          command_parse_result = parse_path_command(memory, last_command, c, c, end_f, &path, current_point, origin);
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


// NOTE: Only implements translate()
V2
parse_svg_transform(String transform_attr)
{
  V2 result = {0, 0};

  char *c = transform_attr.text;
  char *end_f = transform_attr.text + transform_attr.length;

  consume_whitespace(c, end_f);
  if (str_eq(c, String("translate")))
  {
    consume_until_char(c, '(', end_f);
    char *start_translate = c;

    consume_until_char(c, ')', end_f);
    char *end_translate = c;

    if (start_translate == end_f || end_translate == end_f)
    {
      log(L_SVG, "Invalid translate() in transform");
    }
    else
    {
      c = start_translate + 1;

      consume_until(c, is_num_or_sign, end_translate);
      c = get_num(c, end_translate, &result.x);

      if (c == end_translate)
      {
        log(L_SVG, "Invalid translate: No dx provided");
      }
      else
      {
        consume_until(c, is_num_or_sign, end_translate);
        c = get_num(c, end_translate, &result.y);
      }
    }
  }

  return result;
}


V2
parse_svg_group(XMLTag *g_tag)
{
  V2 result = {0, 0};

  String transform_attr = get_attr_value(g_tag, String("transform"));
  if (transform_attr.text != 0)
  {
    result = parse_svg_transform(transform_attr);
  }

  return result;
}


void
get_svg_operations(Memory *memory, XMLTag *tag, SVGOperation **result, V2 delta = (V2){0, 0})
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

      (*result)->path = parse_path(memory, child, delta);
    }
    else if (str_eq(child->name, String("g")))
    {
      delta += parse_svg_group(child);

      get_svg_operations(memory, child, result, delta);
    }
    else if (str_eq(child->name, String("svg")))
    {
      get_svg_operations(memory, child, result, delta);
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