void
debug_render_font_outline(GLuint general_screen_vao, ScreenSpaceRendering *screen_space_rendering, GeneralVertices *general_vertices, VBO_Segment vbo_segment)
{
  glUseProgram(screen_space_rendering->shader_program);
  glBindVertexArray(general_screen_vao);

  glBindBuffer(general_vertices->vbo.binding_target, general_vertices->vbo.id);

  glDrawArrays(GL_TRIANGLES, vbo_segment.start_position, vbo_segment.n_elements);

  glBindVertexArray(0);
  glUseProgram(0);
  glBindBuffer(general_vertices->vbo.binding_target, 0);
}