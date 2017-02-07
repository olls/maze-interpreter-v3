void
draw_car(GameState *game_state, RenderOperations *render_operations, RenderBasis *render_basis, Car *car, u64 time_us, V4 colour = (V4){1, 0.60, 0.13, 0.47})
{
  u32 car_radius = calc_car_radius(game_state->cell_spacing, game_state->cell_margin);
  V2 pos = cell_coord_to_world(game_state->cell_spacing, car->target_cell_x, car->target_cell_y) + car->offset;

  add_circle_to_render_list(render_operations, render_basis, pos, car_radius, colour);

  u32 max_len = 16;
  char str[max_len];
  r32 font_size = 0.15;

  u32 chars = fmted_str(str, max_len, "%d", car->value);
  font_size /= chars;

  draw_string(render_operations, render_basis, &game_state->bitmaps.font, pos - 0.5*(V2){chars, 1}*CHAR_SIZE*game_state->world_per_pixel*font_size, str, font_size);
}


void
draw_cars(GameState *game_state, RenderOperations *render_operations, RenderBasis *render_basis, Cars *cars, u64 time_us)
{
  // TODO: Loop through only relevant cars?
  //       i.e.: spacial partitioning the storage.
  //       Store the cars in the quad-tree?
  CarsIterator iter = {};
  Car *car;
  while ((car = cars_iterator(cars, &iter)))
  {
#ifdef DEBUG_BLOCK_COLORS
    draw_car(game_state, render_operations, render_basis, car, time_us, iter.cars_block->c);
#else
    draw_car(game_state, render_operations, render_basis, car, time_us);
#endif
  }
}


void
draw_svg_path(RenderOperations *render_operations, RenderBasis *render_basis, V2 origin, SVGPath *path, SVGStyle *style)
{
  V4 colour = style->stroke_colour;
  r32 width = style->stroke_width;
  LineEndStyle line_end = svg_linecap_to_line_end(style->stroke_linecap);
  LineEndStyle line_join = svg_linejoin_to_line_end(style->stroke_linejoin);

  for (u32 segment_n = 0;
       segment_n < path->n_segments;
       ++segment_n)
  {
    LineSegment *segment = path->segments + segment_n;

    V2 start = segment->start;
    V2 end = segment->end;

    start += origin;
    end   += origin;

    LineEndStyle this_line_cap_start = line_join;
    LineEndStyle this_line_cap_end = line_join;

    if (segment_n == 0)
    {
      this_line_cap_start = line_end;
    }
    if (segment_n == path->n_segments-1)
    {
      this_line_cap_end = line_end;
    }

    add_line_to_render_list(render_operations, render_basis, start, end, colour, width, this_line_cap_start, this_line_cap_end);
  }
}


void
draw_svg(RenderOperations *render_operations, RenderBasis *render_basis, V2 origin, SVGOperation *svg_operations)
{
  SVGOperation *operation = svg_operations;
  while (operation)
  {
    switch (operation->type)
    {
      case (SVG_OP_PATH):
      {
        draw_svg_path(render_operations, render_basis, origin, &operation->path, &operation->style);
      } break;

      case (SVG_OP_RECT):
      {
        SVGRect *rect = &operation->rect;
        add_box_outline_to_render_list(render_operations, render_basis, origin + rect->rect, operation->style.stroke_colour, operation->style.stroke_width, rect->radius);
      } break;

      case (SVG_OP_CIRCLE):
      {
        SVGCircle *circle = &operation->circle;
        add_circle_to_render_list(render_operations, render_basis, origin + circle->position, circle->radius, operation->style.stroke_colour, operation->style.stroke_width);
      } break;

      case (SVG_OP_LINE):
      {
        SVGLine *line = &operation->line;
        add_line_to_render_list(render_operations, render_basis, origin + line->line.start, origin + line->line.end, operation->style.stroke_colour, operation->style.stroke_width, svg_linecap_to_line_end(operation->style.stroke_linecap));
      } break;

      default:
      {
        invalid_code_path;
      } break;
    }

    operation = operation->next;
  }
}