struct OpenGLShaderProgram
{
  GLuint program;

  GLint attribute_world_cell_position_x;
  GLint attribute_world_cell_position_y;
  GLint attribute_world_cell_offset;
  GLint attribute_colour;

  GLint uniform_int_render_origin_cell_x;
  GLint uniform_int_render_origin_cell_y;
  GLint uniform_vec2_render_origin_offset;
  GLint uniform_float_scale;
};