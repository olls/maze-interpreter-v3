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
draw_svg_path(RenderOperations *render_operations, RenderBasis *render_basis, V2 origin, SVGPath *path)
{
  V4 colour = path->style.stroke_colour;
  r32 width = path->style.stroke_width;

  for (u32 segment_n = 0;
       segment_n < path->n_segments;
       ++segment_n)
  {
    LineSegment *segment = path->segments + segment_n;

    V2 start = segment->start;
    V2 end = segment->end;

    start += origin;
    end   += origin;

    add_line_to_render_list(render_operations, render_basis, start, end, colour, width);
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
        draw_svg_path(render_operations, render_basis, origin, &operation->path);
      } break;

      case (SVG_OP_RECT):
      {
        SVGRect *rect = &operation->rect;
        add_box_outline_to_render_list(render_operations, render_basis, origin + rect->rect, rect->style.stroke_colour, rect->style.stroke_width);
      } break;

      default:
      {
        invalid_code_path;
      } break;
    }

    operation = operation->next;
  }
}