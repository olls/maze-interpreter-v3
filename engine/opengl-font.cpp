void
debug_render_font_outline(GLuint screen_shader, GLuint vao, OpenGL_Buffer *vbo, u32 vbo_start_pos, u32 elements)
{
  glUseProgram(screen_shader);
  glBindVertexArray(vao);
  glBindBuffer(vbo->binding_target, vbo->id);

  glDrawArrays(GL_LINE_LOOP, vbo_start_pos, elements);

  glBindVertexArray(0);
  glUseProgram(0);
  glBindBuffer(vbo->binding_target, 0);
}