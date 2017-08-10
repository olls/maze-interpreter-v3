// General VBO can be used for arbitrary vertex data, items are kept track of with VBO_Segments, rendering for each item is managed by the user

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
VBO_Segment
add_vertex_buffer_to_general_vbo,
{
  VBO_Segment result;

  result.start_position = add_vertex_buffer(channel, &general_vertices->vbo, vertex_buffer);
  result.n_elements = vertex_buffer->n_vertices;

  return result;
},
GeneralVertices *general_vertices, VertexBuffer *vertex_buffer)


VBO_Segment
add_glyph_to_general_vertices(Font *font, GeneralVertices *general_vertices, Memory *memory, u32 character)
{
  VBO_Segment result;

  VertexBuffer vertices = {};
  get_glyph(font, character, memory, &vertices);

  result = add_vertex_buffer_to_general_vbo(L_OpenType, general_vertices, &vertices);

  return result;
}