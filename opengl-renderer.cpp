void
gl_init()
{
    // glShadeModel(GL_SMOOTH);
    glClearColor(1, 1, 1, 1);
    // glClearDepth(1.0f);
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LEQUAL);
    // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
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


bool
initialise_vbo(OpenGLShaderProgram *shader_program, GLuint *vbo_result)
{
  bool success = true;

  // Create buffer
  u32 n_buffers = 1;
  glGenBuffers(n_buffers, vbo_result);

  // Bind this buffer as the ARRAY_BUFFER
  glBindBuffer(GL_ARRAY_BUFFER, *vbo_result);

  // Specify data format
  GLintptr base_offset = 0;
  u32 binding_index = 0;
  glBindVertexBuffer(binding_index, *vbo_result, base_offset, sizeof(ShaderAttributes));

  // Bind the vertex shader attributes
  glVertexAttribFormat(shader_program->attribute_world_cell_position_x, 1, GL_INT, GL_FALSE, offsetof(ShaderAttributes, world_cell_position_x));
  glVertexAttribBinding(shader_program->attribute_world_cell_position_x, binding_index);

  glVertexAttribFormat(shader_program->attribute_world_cell_position_y, 1, GL_INT, GL_FALSE, offsetof(ShaderAttributes, world_cell_position_y));
  glVertexAttribBinding(shader_program->attribute_world_cell_position_y, binding_index);

  glVertexAttribFormat(shader_program->attribute_world_cell_offset, 2, GL_FLOAT, GL_FALSE, offsetof(ShaderAttributes, world_cell_offset));
  glVertexAttribBinding(shader_program->attribute_world_cell_offset, binding_index);

  glVertexAttribFormat(shader_program->attribute_colour, 4, GL_FLOAT, GL_FALSE, offsetof(ShaderAttributes, colour));
  glVertexAttribBinding(shader_program->attribute_colour, binding_index);

  return success;
}


void
load_coords_into_vbo(GLuint vbo_id, ShaderAttributes coords[], u32 n_coords)
{
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
  glBufferData(GL_ARRAY_BUFFER, n_coords * sizeof(ShaderAttributes), coords, GL_STREAM_DRAW);
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