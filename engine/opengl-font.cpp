void
add_glyph_to_general_vertices(Font *font, GeneralVertices *general_vertices, Memory *frame_memory, u32 character,
                              GL_BufferSegment *result_vbo_segment, GL_BufferSegment *result_ibo_segment)
{
  VertexArray outline = {};
  get_glyph(font, character, frame_memory, &outline);

  *result_vbo_segment = add_vertex_array_to_general_vertices(L_Font, general_vertices, &outline);

  u32 n_indices;
  u16 *indices_array = outline_to_triangles(frame_memory, outline, &n_indices);

  *result_ibo_segment = add_indices_to_general_vertices(L_Font, general_vertices, indices_array, n_indices);
}


void
debug_render_font_outline(GLuint general_screen_vao, ScreenSpaceRendering *screen_space_rendering,
                          GeneralVertices *general_vertices, GL_BufferSegment vbo_segment, GL_BufferSegment ibo_segment)
{
  glUseProgram(screen_space_rendering->shader_program);
  glBindVertexArray(general_screen_vao);

  glBindBuffer(general_vertices->vbo.binding_target, general_vertices->vbo.id);
  glBindBuffer(general_vertices->ibo.binding_target, general_vertices->ibo.id);

  glDrawElements(GL_TRIANGLES, ibo_segment.n_elements, GL_UNSIGNED_SHORT, (GLvoid *)(uintptr_t)(ibo_segment.start_position * general_vertices->ibo.element_size));

  glBindVertexArray(0);
  glUseProgram(0);
  glBindBuffer(general_vertices->vbo.binding_target, 0);
}