// General VBO can be used for arbitrary vertex data, items are kept track of with GL_BufferSegments, rendering for each item is managed by the user

// TODO: Do we want to add colours (more...) to the general vbo??


void
setup_general_vertices(GeneralVertices *general_vertices)
{
  general_vertices->vbo.id = create_buffer();
  general_vertices->vbo.element_size = sizeof(vec2);
  general_vertices->vbo.total_elements = 0;
  general_vertices->vbo.elements_used = 0;
  general_vertices->vbo.binding_target = GL_ARRAY_BUFFER;
  general_vertices->vbo.usage = GL_STATIC_DRAW;
  general_vertices->vbo.setup_attributes_func = 0;

  general_vertices->ibo.id = create_buffer();
  general_vertices->ibo.element_size = sizeof(u16);
  general_vertices->ibo.total_elements = 0;
  general_vertices->ibo.elements_used = 0;
  general_vertices->ibo.binding_target = GL_ELEMENT_ARRAY_BUFFER;
  general_vertices->ibo.usage = GL_STATIC_DRAW;
  general_vertices->ibo.setup_attributes_func = 0;
}


GLuint
setup_general_vertices_to_screen_space_vao(GeneralVertices *general_vertices)
{
  GLuint result = create_vao();
  glBindVertexArray(result);

  glBindBuffer(general_vertices->vbo.binding_target, general_vertices->vbo.id);

  GLuint attribute_pos = 12;
  glEnableVertexAttribArray(attribute_pos);
  glVertexAttribPointer(attribute_pos, sizeof(vec2)/sizeof(r32), GL_FLOAT, GL_FALSE, 0, 0);

  glBindVertexArray(0);

  return result;
}


POLYMORPHIC_LOGGING_FUNCTION(
GL_BufferSegment
add_vertex_array_to_general_vertices,
{
  GL_BufferSegment result;

  result.start_position = add_vertex_array_to_gl_buffer(channel, &general_vertices->vbo, vertex_array);
  result.n_elements = vertex_array->n_vertices;

  return result;
},
GeneralVertices *general_vertices, VertexArray *vertex_array)


POLYMORPHIC_LOGGING_FUNCTION(
GL_BufferSegment
add_indices_to_general_vertices,
{
  GL_BufferSegment result;

  result.start_position = add_items_to_gl_buffer(channel, &general_vertices->ibo, n_indices, indices);
  result.n_elements = n_indices;

  return result;
},
GeneralVertices *general_vertices, u16 *indices, u32 n_indices)