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
  // TODO: Alpha!
  glColor3f(color.r, color.g, color.b);
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