void
gl_init()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(1, 1, 1, 1);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}


u32
gl_set_viewport(u32 width, u32 height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);

    return 1;
}


void
load_matrix(OpenGLProjection *matrices)
{
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(matrices->projection);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(matrices->modelview);
}


V2
un_project_mouse(OpenGLViewport viewport, OpenGLProjection *projection, Renderer *renderer, Mouse *mouse)
{
  V2 gl_mouse_coord = {mouse->x, renderer->height - mouse->y};

  GLdouble mouse_pixel_z;
  glReadPixels(gl_mouse_coord.x, gl_mouse_coord.y, 1 , 1, GL_DEPTH_COMPONENT, GL_DOUBLE, &mouse_pixel_z);

  GLdouble mouse_world_x,
           mouse_world_y,
           mouse_world_z;
  gluUnProject(gl_mouse_coord.x, gl_mouse_coord.y, mouse_pixel_z,
               projection->modelview, projection->projection, viewport,
               &mouse_world_x, &mouse_world_y, &mouse_world_z);

  V2 mouse_world = {mouse_world_x, mouse_world_y};
  return mouse_world;
}


void
gl_set_color(V4 color)
{
  glColor3f(color.r, color.g, color.b);
}


void
draw_circle(V2 pos,
            r32 radius,
            V4 color,
            r32 world_outline_width = -1,
            u32 corners = 0b1111)
{
  // NOTE: Deprecated
}


void
draw_circle_segment(r32 start, r32 end, r32 stroke_plus, r32 stroke_minus)
{
  V2 seg_start = {sin(start), cos(start)};
  V2 seg_end   = {sin(end), cos(end)};
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
draw_box(Rectangle box, V4 color)
{
  // NOTE: Deprecated

  glPushMatrix();

  glColor3f(color.r, color.g, color.b);

  glBegin(GL_QUADS);
    glVertex3f(box.start.x, box.start.y, 0);
    glVertex3f(box.end.x,   box.start.y, 0);
    glVertex3f(box.end.x,   box.end.y,   0);
    glVertex3f(box.start.x, box.end.y,   0);
  glEnd();

  glPopMatrix();
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
draw_line(V2 start, V2 end, r32 width = 50, LineEndStyle line_end = LINE_END_NULL, LineEndStyle end_line_end = LINE_END_NULL)
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

  V2 line_direction = unit_vector(end - start);
  V2 width_vector = width * 0.5 * line_direction;

  if (line_end == LINE_END_SQUARE)
  {
    start -= width_vector;
  }
  if (end_line_end == LINE_END_SQUARE)
  {
    end += width_vector;
  }

  V2 width_comp = vector_tangent(width_vector);

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
get_default_blit_bitmap_options(BlitBitmapOptions *opts)
{
  opts->crop = (Rectangle){-1};
  opts->color_multiplier = (V4){1, 1, 1, 1};
  opts->invert = false;
  opts->rotate = 0;
  opts->hue_shift = 0;
  opts->interpolation = false;
  opts->scale = 1;
}

void
draw_bitmap(Bitmap *bitmap,
            V2 pos,
            BlitBitmapOptions *opts = 0)
{
  BlitBitmapOptions opts_;
  if (opts == 0)
  {
    get_default_blit_bitmap_options(&opts_);
    opts = &opts_;
  }


}