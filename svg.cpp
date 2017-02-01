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
parse_path_d(Memory *memory, XMLTag *path_tag, String d_attr, V2 origin)
{
  SVGPath path = {.segments = 0, .n_segments = 0};

  log(L_SVG, "Parsing d: '%.*s'", d_attr.length, d_attr.text);
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

  return path;
}


b32
is_style_value_char(char character)
{
  return is_letter(character) ||
         is_num_or_sign(character) ||
         character == '.' ||
         character == '#';
}


char *
find_svg_style(char *c, char *end, String *label_result, String *value_result)
{
  consume_whitespace_nl(c, end);
  char *start_label = c;

  consume_while_f_or_char(c, is_letter, '-', end);
  char *end_label = c;

  *label_result = (String){
    .text = start_label,
    .length = end_label - start_label
  };

  consume_until_char(c, ':', end);
  ++c;

  consume_whitespace(c, end);
  char *start_value = c;

  consume_while(c, is_style_value_char, end);
  char *end_value = c;

  *value_result = (String){
    .text = start_value,
    .length = end_value - start_value
  };

  consume_until_char(c, ';', end);
  ++c;

  return c;
}


b32
parse_colour_string(String string, V4 *result)
{
  b32 parsed = true;

  if (string.text[0] == '#')
  {
    // UNIMPLEMENTED: Named colours, eg: red, green, fuchsia

    if (string.length == 4)
    {
      // #rgb
      String channel_str = {.length = 1};

      channel_str.text = string.text + 1;
      result->r = (1.0/15.0) * (r32)hex_string_to_int(channel_str);

      channel_str.text = string.text + 2;
      result->g = (1.0/15.0) * (r32)hex_string_to_int(channel_str);

      channel_str.text = string.text + 3;
      result->b = (1.0/15.0) * (r32)hex_string_to_int(channel_str);
    }
    else if (string.length == 7)
    {
      // #rrggbb
      String channel_str = {.length = 2};

      channel_str.text = string.text + 1;
      result->r = (1.0/255.0) * (r32)hex_string_to_int(channel_str);

      channel_str.text = string.text + 3;
      result->g = (1.0/255.0) * (r32)hex_string_to_int(channel_str);

      channel_str.text = string.text + 5;
      result->b = (1.0/255.0) * (r32)hex_string_to_int(channel_str);
    }
    else
    {
      parsed = false;
    }
  }
  else
  {
    parsed = false;
  }

  return parsed;
}


SVGStrokeLinecap
parse_svg_stroke_linecap(String value)
{
  SVGStrokeLinecap result = LINECAP_BUTT;

  if (str_eq(value, String("butt")))
  {
    result = LINECAP_BUTT;
  }
  else if (str_eq(value, String("round")))
  {
    result = LINECAP_ROUND;
  }
  else if (str_eq(value, String("square")))
  {
    result = LINECAP_SQUARE;
  }

  return result;
}


SVGStrokeLinejoin
parse_svg_stroke_linejoin(String value)
{
  SVGStrokeLinejoin result = LINEJOIN_MITER;

  if (str_eq(value, String("miter")))
  {
    result = LINEJOIN_MITER;
  }
  else if (str_eq(value, String("round")))
  {
    result = LINEJOIN_ROUND;
  }
  else if (str_eq(value, String("square")))
  {
    result = LINEJOIN_SQUARE;
  }

  return result;
}


void
parse_svg_style(String label, String value, SVGStyle *result)
{
  if (str_eq(label, String("fill")))
  {
    log(L_SVG, "Style label: fill");

    result->fill_colour = (V4){0, 0, 0, 0};
    result->filled = parse_colour_string(value, &result->fill_colour);
  }
  else if (str_eq(label, String("fill-opacity")))
  {
    log(L_SVG, "Style label: fill-opacity");

    get_num(value.text, value.text + value.length, &result->fill_colour.a);
  }
  else if (str_eq(label, String("stroke")))
  {
    log(L_SVG, "Style label: stroke");

    result->stroke_colour = (V4){0, 0, 0, 0};
    parse_colour_string(value, &result->stroke_colour);
  }
  else if (str_eq(label, String("stroke-opacity")))
  {
    log(L_SVG, "Style label: stroke-opacity");

    get_num(value.text, value.text + value.length, &result->stroke_colour.a);
  }
  else if (str_eq(label, String("stroke-width")))
  {
    log(L_SVG, "Style label: stroke-width");

    get_num(value.text, value.text + value.length, &result->stroke_width);
  }
  else if (str_eq(label, String("stroke-linecap")))
  {
    log(L_SVG, "Style label: stroke-linecap");

    result->stroke_linecap = parse_svg_stroke_linecap(value);
  }
  else if (str_eq(label, String("stroke-linejoin")))
  {
    log(L_SVG, "Style label: stroke-linejoin");

    result->stroke_linejoin = parse_svg_stroke_linejoin(value);
  }
}


void
parse_svg_styles(String style, SVGStyle *result)
{
  log(L_SVG, "Parsing style: %.*s", style.length, style.text);

  char *c = style.text;
  char *end = style.text + style.length;

  while (c < end)
  {
    String label, value;
    c = find_svg_style(c, end, &label, &value);

    parse_svg_style(label, value, result);
  }

  log(L_SVG, "Styles:");

  log(L_SVG, "stroke_width: %d", result->stroke_width);
  log(L_SVG, "stroke_colour: (%f,%f,%f,%f)", result->stroke_colour.a, result->stroke_colour.r, result->stroke_colour.g, result->stroke_colour.b);
  log(L_SVG, "stroke_linecap: %d", result->stroke_linecap);
  log(L_SVG, "stroke_linejoin: %d", result->stroke_linejoin);
  log(L_SVG, "filled: %d", result->filled);
  log(L_SVG, "fill_colour: (%f,%f,%f,%f)", result->fill_colour.a, result->fill_colour.r, result->fill_colour.g, result->fill_colour.b);
}


SVGPath
parse_path(Memory *memory, XMLTag *path_tag, V2 origin)
{
  SVGPath result;

  String d_attr = get_attr_value(path_tag, String("d"));
  if (d_attr.text != 0)
  {
    result = parse_path_d(memory, path_tag, d_attr, origin);
  }

  String style_attr = get_attr_value(path_tag, String("style"));
  if (style_attr.text != 0)
  {
    parse_svg_styles(style_attr, &result.style);
  }

  return result;
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


SVGRect
parse_rect(Memory *memory, XMLTag *rect_tag, V2 origin)
{
  SVGRect result;

  String transform_attr = get_attr_value(rect_tag, String("transform"));
  if (transform_attr.text != 0)
  {
    origin += parse_svg_transform(transform_attr);
  }

  result.rect = (Rectangle){ .start = origin, .end = origin};

  String x_str = get_attr_value(rect_tag, String("x"));
  String y_str = get_attr_value(rect_tag, String("y"));
  String width_str  = get_attr_value(rect_tag, String("width"));
  String height_str = get_attr_value(rect_tag, String("height"));

  V2 pos, size;
  get_num(x_str.text, x_str.text + x_str.length, &pos.x);
  get_num(y_str.text, y_str.text + y_str.length, &pos.y);
  get_num( width_str.text,  width_str.text +  width_str.length, &size.x);
  get_num(height_str.text, height_str.text + height_str.length, &size.y);

  result.rect.start = origin + pos;
  result.rect.end = result.rect.start + size;

  String style_attr = get_attr_value(rect_tag, String("style"));
  if (style_attr.text != 0)
  {
    parse_svg_styles(style_attr, &result.style);
  }

  return result;
}


SVGCircle
parse_circle(Memory *memory, XMLTag *circle_tag, V2 origin)
{
  SVGCircle result;

  String transform_attr = get_attr_value(circle_tag, String("transform"));
  if (transform_attr.text != 0)
  {
    origin += parse_svg_transform(transform_attr);
  }

  String x_str = get_attr_value(circle_tag, String("cx"));
  String y_str = get_attr_value(circle_tag, String("cy"));
  String radius_str  = get_attr_value(circle_tag, String("r"));

  V2 pos;
  r32 radius;
  get_num(x_str.text, x_str.text + x_str.length, &pos.x);
  get_num(y_str.text, y_str.text + y_str.length, &pos.y);
  get_num(radius_str.text, radius_str.text + radius_str.length, &radius);

  result.position = origin + pos;
  result.radius = radius;

  String style_attr = get_attr_value(circle_tag, String("style"));
  if (style_attr.text != 0)
  {
    parse_svg_styles(style_attr, &result.style);
  }

  return result;
}


SVGLine
parse_line(Memory *memory, XMLTag *line_tag, V2 origin)
{
  SVGLine result;

  String transform_attr = get_attr_value(line_tag, String("transform"));
  if (transform_attr.text != 0)
  {
    origin += parse_svg_transform(transform_attr);
  }

  String x1_str = get_attr_value(line_tag, String("x1"));
  String y1_str = get_attr_value(line_tag, String("y1"));
  String x2_str = get_attr_value(line_tag, String("x2"));
  String y2_str = get_attr_value(line_tag, String("y2"));

  log(L_SVG, "%.*s", x1_str.length, x1_str.text);
  log(L_SVG, "%.*s", y1_str.length, y1_str.text);
  log(L_SVG, "%.*s", x2_str.length, x2_str.text);
  log(L_SVG, "%.*s", y2_str.length, y2_str.text);

  V2 start, end;
  get_num(x1_str.text, x1_str.text + x1_str.length, &start.x);
  get_num(y1_str.text, y1_str.text + y1_str.length, &start.y);
  get_num(x2_str.text, x2_str.text + x2_str.length,   &end.x);
  get_num(y2_str.text, y2_str.text + y2_str.length,   &end.y);

  result.line.start = origin + start;
  result.line.end = origin + end;

  String style_attr = get_attr_value(line_tag, String("style"));
  if (style_attr.text != 0)
  {
    parse_svg_styles(style_attr, &result.style);
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


SVGRect
get_svg_root_rect(XMLTag *root, V2 origin)
{
  SVGRect result;
  result.rect = (Rectangle){ .start = origin, .end = origin};

  String width_str  = get_attr_value(root, String("width"));
  String height_str = get_attr_value(root, String("height"));

  V2 size;
  get_num( width_str.text,  width_str.text +  width_str.length, &size.x);
  get_num(height_str.text, height_str.text + height_str.length, &size.y);

  result.rect.end += size;
  result.style = (SVGStyle){
    .stroke_width = 1,
    .stroke_colour = (V4){1, 1, 1, 0},
    .stroke_linecap = LINECAP_SQUARE,
    .stroke_linejoin = LINEJOIN_SQUARE,
    .filled = false
  };

  return result;
}


void
get_svg_operations(Memory *memory, XMLTag *tag, SVGOperation **result, V2 delta = (V2){0, 0})
{
  // TODO: Parse the style and transform attributes in here instead of separately for each type

  XMLTag *child = tag->first_child;
  while (child)
  {
    if (str_eq(child->name, String("path")))
    {
      log(L_SVG, "Found: path");

      SVGOperation *old_start = *result;
      *result = push_struct(memory, SVGOperation);

      (*result)->type = SVG_OP_PATH;
      (*result)->next = old_start;

      (*result)->path = parse_path(memory, child, delta);
    }
    else if (str_eq(child->name, String("rect")))
    {
      log(L_SVG, "Found: rect");

      SVGOperation *old_start = *result;
      *result = push_struct(memory, SVGOperation);

      (*result)->type = SVG_OP_RECT;
      (*result)->next = old_start;

      (*result)->rect = parse_rect(memory, child, delta);
    }
    else if (str_eq(child->name, String("circle")))
    {
      log(L_SVG, "Found: circle");

      SVGOperation *old_start = *result;
      *result = push_struct(memory, SVGOperation);

      (*result)->type = SVG_OP_CIRCLE;
      (*result)->next = old_start;

      (*result)->circle = parse_circle(memory, child, delta);
    }
    else if (str_eq(child->name, String("line")))
    {
      log(L_SVG, "Found: line");

      SVGOperation *old_start = *result;
      *result = push_struct(memory, SVGOperation);

      (*result)->type = SVG_OP_LINE;
      (*result)->next = old_start;

      (*result)->line = parse_line(memory, child, delta);
    }
    else if (str_eq(child->name, String("g")))
    {
      log(L_SVG, "Found: g");

      delta += parse_svg_group(child);

      get_svg_operations(memory, child, result, delta);
    }
    else if (str_eq(child->name, String("svg")))
    {
      log(L_SVG, "Found: svg");

      SVGOperation *old_start = *result;
      *result = push_struct(memory, SVGOperation);

      (*result)->type = SVG_OP_RECT;
      (*result)->next = old_start;

      (*result)->rect = get_svg_root_rect(child, delta);

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

    current = current->next;
  }
}