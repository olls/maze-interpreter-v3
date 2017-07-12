
// TODO: Separate instance types for the different cell types.


void
setup_cell_vertex_vbo_and_ibo(OpenGL_VBOs *opengl_vbos, const vec2 *vertices, u32 n_vertices,
                                                        const GLushort *indices, u32 n_indices)
{
  // Vertex VBO

  glGenBuffers(1, &opengl_vbos->cell_vertex_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, opengl_vbos->cell_vertex_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * n_vertices, vertices, GL_STATIC_DRAW);

  GLuint attribute_vertex = 8;
  glVertexAttribPointer(attribute_vertex, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
  glEnableVertexAttribArray(attribute_vertex);

  // Vertex IBO

  glGenBuffers(1, &opengl_vbos->cell_vertex_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, opengl_vbos->cell_vertex_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * n_indices, indices, GL_STATIC_DRAW);
  opengl_vbos->n_cell_indices = n_indices;
}


void
setup_cell_instances_vbo(OpenGL_VBOs *opengl_vbos)
{
  glGenBuffers(1, &opengl_vbos->cell_instances_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, opengl_vbos->cell_instances_vbo);

  GLuint attribute_world_cell_position_x = 0;
  glEnableVertexAttribArray(attribute_world_cell_position_x);
  glVertexAttribPointer(attribute_world_cell_position_x, 1, GL_INT, GL_FALSE, sizeof(CellInstance), (void *)offsetof(CellInstance, world_cell_position_x));
  glVertexAttribDivisor(attribute_world_cell_position_x, 1);

  GLuint attribute_world_cell_position_y = 1;
  glEnableVertexAttribArray(attribute_world_cell_position_y);
  glVertexAttribPointer(attribute_world_cell_position_y, 1, GL_INT, GL_FALSE, sizeof(CellInstance), (void *)offsetof(CellInstance, world_cell_position_y));
  glVertexAttribDivisor(attribute_world_cell_position_y, 1);

  GLuint attribute_world_cell_offset = 2;
  glEnableVertexAttribArray(attribute_world_cell_offset);
  glVertexAttribPointer(attribute_world_cell_offset, sizeof(vec2)/sizeof(r32), GL_FLOAT, GL_FALSE, sizeof(CellInstance), (void *)offsetof(CellInstance, world_cell_offset));
  glVertexAttribDivisor(attribute_world_cell_offset, 1);

  GLuint attribute_colour = 4;
  glEnableVertexAttribArray(attribute_colour);
  glVertexAttribPointer(attribute_colour, sizeof(vec4)/sizeof(r32), GL_FLOAT, GL_FALSE, sizeof(CellInstance), (void *)offsetof(CellInstance, colour));
  glVertexAttribDivisor(attribute_colour, 1);
}


void
load_cell_instances(OpenGL_VBOs *opengl_vbos, CellInstance *cell_instances, u32 n_cell_instances)
{
  glBindBuffer(GL_ARRAY_BUFFER, opengl_vbos->cell_instances_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CellInstance) * n_cell_instances, cell_instances, GL_STATIC_DRAW);
  opengl_vbos->n_cell_instances = n_cell_instances;
}


void
update_cell_instance(OpenGL_VBOs *opengl_vbos, u32 cell_instance_number, CellInstance *instance)
{
  glBindBuffer(GL_ARRAY_BUFFER, opengl_vbos->cell_instances_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(CellInstance) * cell_instance_number, sizeof(CellInstance), instance);
}


b32
setup_cell_instancing(GameState *game_state)
{
  b32 success = true;

  const char *filenames[] = {"vertex-shader.glvs", "fragment-shader.glfs"};
  GLenum shader_types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

  success &= create_shader_program(filenames, shader_types, array_count(shader_types), &game_state->shader_program);
  glUseProgram(game_state->shader_program);

  setup_vao();
  setup_cell_vertex_vbo_and_ibo(&game_state->opengl_vbos, CELL_VERTICES, array_count(CELL_VERTICES),
                                                          CELL_TRIANGLE_INDICES, array_count(CELL_TRIANGLE_INDICES));
  setup_cell_instances_vbo(&game_state->opengl_vbos);

  CellInstance cell_instances[] = {
    {.world_cell_position_x = 0, .world_cell_position_y = 0, .world_cell_offset = {-0.0, -0.0}, .colour = {1, 0, 0, 1}},
    {.world_cell_position_x = 0, .world_cell_position_y = 0, .world_cell_offset = {-1.0, -0.0}, .colour = {0, 1, 0, 1}},
    {.world_cell_position_x = 0, .world_cell_position_y = 0, .world_cell_offset = {-1.0, -1.0}, .colour = {0, 0, 1, 1}},
    {.world_cell_position_x = 0, .world_cell_position_y = 0, .world_cell_offset = {-0.0, -1.0}, .colour = {1, 0, 1, 1}}
  };
  load_cell_instances(&game_state->opengl_vbos, cell_instances, array_count(cell_instances));

  game_state->uniforms.int_render_origin_cell_x.name = "render_origin_cell_x";
  game_state->uniforms.int_render_origin_cell_y.name = "render_origin_cell_y";
  game_state->uniforms.vec2_render_origin_offset.name = "render_origin_offset";
  game_state->uniforms.float_scale.name = "scale";
  success &= get_uniform_locations(game_state->shader_program, game_state->uniforms.array, array_count(game_state->uniforms.array));

  success &= print_gl_errors();
  printf("OpenGL VBOs setup finished\n");

  return success;
}