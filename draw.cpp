void
draw_box_outline(Rectangle box, V4 color, r32 thickness = 1, r32 radius = 0)
{
  // Square and rounded box are in separate paths because the rounded version with radius = 0 overlaps the corners of the lines, messing up the alpha.

  if (radius > 0)
  {
    draw_circle((V2){box.end.x - radius, box.start.y + radius}, radius, color, thickness, 0b1000);
    draw_circle(box.end - radius,                               radius, color, thickness, 0b0100);
    draw_circle((V2){box.start.x + radius, box.end.y - radius}, radius, color, thickness, 0b0010);
    draw_circle(box.start + radius,                             radius, color, thickness, 0b0001);

    draw_box((Rectangle){ (V2){ box.start.x+radius , box.start.y         }, (V2){ box.end.x-radius     , box.start.y+thickness } }, color);
    draw_box((Rectangle){ (V2){ box.start.x+radius , box.end.y-thickness }, (V2){ box.end.x-radius     , box.end.y             } }, color);
    draw_box((Rectangle){ (V2){ box.start.x        , box.start.y+radius  }, (V2){ box.start.x+thickness, box.end.y-radius      } }, color);
    draw_box((Rectangle){ (V2){ box.end.x-thickness, box.start.y+radius  }, (V2){ box.end.x            , box.end.y-radius      } }, color);
  }
  else
  {
    draw_box((Rectangle){ (V2){ box.start.x        , box.start.y           }, (V2){ box.end.x            , box.start.y+thickness } }, color);
    draw_box((Rectangle){ (V2){ box.start.x        , box.end.y-thickness   }, (V2){ box.end.x            , box.end.y             } }, color);
    draw_box((Rectangle){ (V2){ box.start.x        , box.start.y+thickness }, (V2){ box.start.x+thickness, box.end.y-thickness   } }, color);
    draw_box((Rectangle){ (V2){ box.end.x-thickness, box.start.y+thickness }, (V2){ box.end.x            , box.end.y-thickness   } }, color);
  }
}


void
draw_box_outline(V2 size, r32 thickness = 1, r32 radius = 0)
{
  r32 corner_ext = -thickness*0.5 + radius;

  if (radius > 0)
  {
    V2 top_left     = -0.5*size + corner_ext;
    V2 top_right    = (V2){ 0.5*size.x-corner_ext, -0.5*size.y+corner_ext};
    V2 bottom_right =  0.5*size - corner_ext;
    V2 bottom_left  = (V2){-0.5*size.x+corner_ext,  0.5*size.y-corner_ext};

    glPushMatrix();
      glTranslatef(top_left.x, top_left.y, 0);
      glScalef(radius-thickness/2, radius-thickness/2, 1);
      draw_circle_outline(thickness/(radius-thickness/2), 0.5, 0.75);
    glPopMatrix();
    glPushMatrix();
      glTranslatef(top_right.x, top_right.y, 0);
      glScalef(radius-thickness/2, radius-thickness/2, 1);
      draw_circle_outline(thickness/(radius-thickness/2), 0.25, 0.5);
    glPopMatrix();
    glPushMatrix();
      glTranslatef(bottom_right.x, bottom_right.y, 0);
      glScalef(radius-thickness/2, radius-thickness/2, 1);
      draw_circle_outline(thickness/(radius-thickness/2), 0, 0.25);
    glPopMatrix();
    glPushMatrix();
      glTranslatef(bottom_left.x, bottom_left.y, 0);
      glScalef(radius-thickness/2, radius-thickness/2, 1);
      draw_circle_outline(thickness/(radius-thickness/2), 0.75, 1);
    glPopMatrix();
  }
  draw_line(-0.5*size + (V2){corner_ext, 0         }, (V2){ 0.5*size.x-corner_ext, -0.5*size.y           }, thickness);
  draw_line(-0.5*size + (V2){0         , corner_ext}, (V2){-0.5*size.x           ,  0.5*size.y-corner_ext}, thickness);
  draw_line( 0.5*size - (V2){corner_ext, 0         }, (V2){-0.5*size.x-corner_ext,  0.5*size.y           }, thickness);
  draw_line( 0.5*size - (V2){0         , corner_ext}, (V2){ 0.5*size.x           , -0.5*size.y+corner_ext}, thickness);
}


void
draw_car(GameState *game_state, RenderWindow *render_window, Car *car, u64 time_us, V4 colour = (V4){1, 0.60, 0.13, 0.47})
{
  r32 car_radius = calc_car_radius(game_state->cell_margin);

  V2 pos = world_coord_to_render_window_coord(render_window, car->cell_pos);

  glPushMatrix();
    glTranslatef(pos.x, pos.y, 0);

    glPushMatrix();
      glScalef(car_radius, car_radius, 1);
      gl_set_color(colour);

      draw_circle();
    glPopMatrix();

    u32 max_len = 16;
    char str[max_len];
    r32 font_size = 0.15;

    u32 chars = fmted_str(str, max_len, "%d", car->value);
    font_size /= chars;

    draw_string(&game_state->bitmaps.font, pos - 0.5*(V2){chars, 1}*CHAR_SIZE*game_state->world_per_pixel*font_size, str, font_size);
  glPopMatrix();
}


void
draw_cars(GameState *game_state, RenderWindow *render_window, Cars *cars, u64 time_us)
{
  // TODO: Loop through only relevant cars?
  //       i.e.: spacial partitioning the storage.
  //       Store the cars in the quad-tree?
  CarsIterator iter = {};
  Car *car;
  while ((car = cars_iterator(cars, &iter)))
  {
#ifdef DEBUG_BLOCK_COLORS
    draw_car(game_state, render_window, car, time_us, iter.cars_block->c);
#else
    draw_car(game_state, render_window, car, time_us);
#endif
  }
}


void
draw_svg_path(SVGPath *path, SVGStyle *style)
{
  gl_set_color(style->stroke_colour);

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

    draw_line(start, end, width, this_line_cap_start, this_line_cap_end);
  }
}


void
draw_svg(SVGOperation *svg_operations)
{
  SVGOperation *operation = svg_operations;
  while (operation)
  {
    switch (operation->type)
    {
      case (SVG_OP_PATH):
      {
        draw_svg_path(&operation->path, &operation->style);
      } break;

      case (SVG_OP_RECT):
      {
        SVGRect *rect = &operation->rect;
        glPushMatrix();
          V2 center = rect->rect.start + size(rect->rect)*0.5;
          glTranslatef(center.x, center.y, 0);
          gl_set_color(operation->style.stroke_colour);

          draw_box_outline(size(rect->rect), operation->style.stroke_width, rect->radius);
        glPopMatrix();
      } break;

      case (SVG_OP_CIRCLE):
      {
        SVGCircle *circle = &operation->circle;
        glPushMatrix();
          glTranslatef(circle->position.x, circle->position.y, 0);
          glScalef(circle->radius, circle->radius, 1);
          gl_set_color(operation->style.stroke_colour);

          r32 stroke_as_fraction_of_radius = operation->style.stroke_width / circle->radius;
          draw_circle_outline(stroke_as_fraction_of_radius);
        glPopMatrix();
      } break;

      case (SVG_OP_LINE):
      {
        SVGLine *line = &operation->line;
        gl_set_color(operation->style.stroke_colour);
        draw_line(line->line.start, line->line.end, operation->style.stroke_width, svg_linecap_to_line_end(operation->style.stroke_linecap));
      } break;

      default:
      {
        invalid_code_path;
      } break;
    }

    operation = operation->next;
  }
}