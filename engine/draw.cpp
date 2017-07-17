void
draw_circle_segment(r32 start, r32 end, r32 stroke_plus, r32 stroke_minus)
{
  vec2 seg_start = {sin(start), cos(start)};
  vec2 seg_end   = {sin(end), cos(end)};
  glBegin(GL_POLYGON);
    glVertex3f( stroke_plus*seg_end.x,    stroke_plus*seg_end.y,   0);
    glVertex3f( stroke_plus*seg_start.x,  stroke_plus*seg_start.y, 0);
    glVertex3f(stroke_minus*seg_start.x, stroke_minus*seg_start.y, 0);
    glVertex3f(stroke_minus*seg_end.x,   stroke_minus*seg_end.y,   0);
  glEnd();
}


void
draw_circle_outline(r32 stroke_width = 0,
                    r32 start_arc = 0,
                    r32 end_arc = 1,
                    r32 stroke_offset = 0)
{
  u32 n_segs = 32;
  r32 seg_size = 2.0*M_PI/n_segs;

  r32 start = start_arc * 2*M_PI;
  r32 end   =   end_arc * 2*M_PI;

  r32 seg_start,
      seg_end;
  for (seg_start = start,
         seg_end = seg_start+seg_size;
       seg_end < end;
       seg_start += seg_size,
         seg_end += seg_size)
  {
    draw_circle_segment(seg_start, seg_end, 1+stroke_offset+(0.5*stroke_width),
                                            1+stroke_offset-(0.5*stroke_width));
  }
  draw_circle_segment(seg_start, end, 1+stroke_offset+(0.5*stroke_width),
                                      1+stroke_offset-(0.5*stroke_width));
}


void
draw_circle(r32 start_arc = 0,
            r32 end_arc = 1)
{
  return draw_circle_outline(1, start_arc, end_arc, -0.5);
}


void
draw_box()
{
  glBegin(GL_QUADS);
    glVertex3f( 0.5,  0.5, 0);
    glVertex3f( 0.5, -0.5, 0);
    glVertex3f(-0.5, -0.5, 0);
    glVertex3f(-0.5,  0.5, 0);
  glEnd();
}


void
draw_box(Rectangle rect, vec4 color)
{
  vec2 radius = 0.5*size(rect);
  vec2 center = rect.start + radius;

  glPushMatrix();
    glColor3f(color.r, color.g, color.b);
    glTranslatef(center.x, center.y, 0);
    glScalef(radius.x, radius.y, 1);

    draw_box();
  glPopMatrix();
}


void
draw_line(vec2 start, vec2 end, r32 width = 50, LineEndStyle line_end = LINE_END_NULL, LineEndStyle end_line_end = LINE_END_NULL)
{
  // glColor3f(color.r, color.g, color.b);

  if (line_end == LINE_END_NULL)
  {
    line_end = LINE_END_BUTT;
  }
  if (end_line_end == LINE_END_NULL)
  {
    end_line_end = line_end;
  }

  vec2 line_direction = unit_vector(end - start);
  vec2 width_vector = width * 0.5 * line_direction;

  if (line_end == LINE_END_SQUARE)
  {
    start -= width_vector;
  }
  if (end_line_end == LINE_END_SQUARE)
  {
    end += width_vector;
  }

  vec2 width_comp = vector_tangent(width_vector);

  glBegin(GL_QUADS);
    glVertex3f(start.x-width_comp.x, start.y-width_comp.y, 0);
    glVertex3f(start.x+width_comp.x, start.y+width_comp.y, 0);
    glVertex3f(  end.x+width_comp.x,   end.y+width_comp.y, 0);
    glVertex3f(  end.x-width_comp.x,   end.y-width_comp.y, 0);
  glEnd();

  if (line_end == LINE_END_ROUND)
  {
    glPushMatrix();
      glTranslatef(start.x, start.y, 0);
      glScalef(width*0.5, width*0.5, 1);

      draw_circle();
    glPopMatrix();
  }

  if (end_line_end == LINE_END_ROUND)
  {
    glPushMatrix();
      glTranslatef(end.x, end.y, 0);
      glScalef(width*0.5, width*0.5, 1);

      draw_circle();
    glPopMatrix();
  }
}


void
draw_box_outline(vec2 size, r32 thickness = 1, r32 radius = 0)
{
  r32 corner_ext = -thickness*0.5 + radius;

  if (radius > 0)
  {
    vec2 top_left     = -0.5*size + corner_ext;
    vec2 top_right    =  0.5*(vec2){ size.x, -size.y} + (vec2){-corner_ext, corner_ext};
    vec2 bottom_right =  0.5*size - corner_ext;
    vec2 bottom_left  =  0.5*(vec2){-size.x,  size.y} + (vec2){corner_ext, -corner_ext};

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
  draw_line(-0.5*size + Vec2(corner_ext, 0), 0.5*Vec2( size.x, -size.y) - Vec2(-corner_ext,  0), thickness);
  draw_line(-0.5*size + Vec2(0, corner_ext), 0.5*Vec2(-size.x,  size.y) - Vec2( 0, -corner_ext), thickness);
  draw_line( 0.5*size - Vec2(corner_ext, 0), 0.5*Vec2(-size.x,  size.y) - Vec2( corner_ext,  0), thickness);
  draw_line( 0.5*size - Vec2(0, corner_ext), 0.5*Vec2( size.x, -size.y) - Vec2( 0,  corner_ext), thickness);
}


void
draw_svg_path(SVGPath *path, SVGStyle *style)
{
  glColor3f(style->stroke_colour.r, style->stroke_colour.g, style->stroke_colour.b);

  r32 width = style->stroke_width;
  LineEndStyle line_end = svg_linecap_to_line_end(style->stroke_linecap);
  LineEndStyle line_join = svg_linejoin_to_line_end(style->stroke_linejoin);

  for (u32 segment_n = 0;
       segment_n < path->n_segments;
       ++segment_n)
  {
    LineSegment *segment = path->segments + segment_n;

    vec2 start = segment->start;
    vec2 end = segment->end;

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
          vec2 center = rect->rect.start + size(rect->rect)*0.5;
          glTranslatef(center.x, center.y, 0);
          glColor3f(operation->style.stroke_colour.r, operation->style.stroke_colour.g, operation->style.stroke_colour.b);

          draw_box_outline(size(rect->rect), operation->style.stroke_width, rect->radius);
        glPopMatrix();
      } break;

      case (SVG_OP_CIRCLE):
      {
        SVGCircle *circle = &operation->circle;
        glPushMatrix();
          glTranslatef(circle->position.x, circle->position.y, 0);
          glScalef(circle->radius, circle->radius, 1);
          glColor3f(operation->style.stroke_colour.r, operation->style.stroke_colour.g, operation->style.stroke_colour.b);

          r32 stroke_as_fraction_of_radius = operation->style.stroke_width / circle->radius;
          draw_circle_outline(stroke_as_fraction_of_radius);
        glPopMatrix();
      } break;

      case (SVG_OP_LINE):
      {
        SVGLine *line = &operation->line;
        glColor3f(operation->style.stroke_colour.r, operation->style.stroke_colour.g, operation->style.stroke_colour.b);
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