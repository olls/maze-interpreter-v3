// Cells storage in instance VBO:
//  - Allocate block of X bytes
//  - Use block for contiguous array of CellInstances
//  - Record CellInstance positions in QuadTree Cells


// TODO: Separate instance types for the different cell types.


void
update_cell_instance(CellInstancing *cell_instancing, u32 cell_instance_position, CellInstance *cell_instance)
{
  update_buffer_element(L_CellInstancing, &cell_instancing->cell_instances_vbo, cell_instance_position, cell_instance);
}


void
add_cell_instance(CellInstancing *cell_instancing, Cell *cell)
{
  CellInstance cell_instance = {
    .world_cell_position_x = (s32)cell->x,
    .world_cell_position_y = (s32)cell->y,
    .world_cell_offset = {0, 0},

    // TODO: This is temporary until we have the different cell types in different instance arrays.
    .colour = get_cell_color(cell->type)
  };

  cell->opengl_instance_position = new_buffer_element(L_CellInstancing, &cell_instancing->cell_instances_vbo, &cell_instance);

  log(L_CellInstancing, "n_cell_instances: %u, out of cell_instances_vbo_size: %u",
      cell_instancing->cell_instances_vbo.elements_used, cell_instancing->cell_instances_vbo.total_elements);
}


void
recurse_adding_all_cell_instances(CellInstancing *cell_instancing, QuadTree *tree)
{
  if (tree)
  {
    for (u32 cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell *cell = tree->cells + cell_index;
      add_cell_instance(cell_instancing, cell);
    }

    recurse_adding_all_cell_instances(cell_instancing, tree->top_right);
    recurse_adding_all_cell_instances(cell_instancing, tree->top_left);
    recurse_adding_all_cell_instances(cell_instancing, tree->bottom_right);
    recurse_adding_all_cell_instances(cell_instancing, tree->bottom_left);
  }
}


void
add_all_cell_instances(CellInstancing *cell_instancing, QuadTree *tree)
{
  glBindVertexArray(cell_instancing->vao);

  recurse_adding_all_cell_instances(cell_instancing, tree);
  print_gl_errors();
  log(L_CellInstancing, "Added all cell instances.");

  glBindVertexArray(0);
}


void
remove_cell_instance(CellInstancing *cell_instancing, Maze *maze, Cell *cell)
{
  OpenGL_Buffer *cell_instances_vbo = &cell_instancing->cell_instances_vbo;

  u32 cell_instance_position = cell->opengl_instance_position;
  cell->opengl_instance_position = INVALID_GL_BUFFER_ELEMENT_POSITION;

  log(L_CellInstancing, "Attempting to remove cell instance %u.", cell_instance_position);

  remove_buffer_element(cell_instances_vbo, cell_instance_position);

  // If the removed element is less or equal than the new number of
  //   elements, then another element has been moved into it's
  //   position, and the Cell needs to know it's new position.

  if (cell_instance_position <= cell_instances_vbo->elements_used)
  {
    log(L_CellInstancing, "Updating moved cell instance");

    glBindBuffer(cell_instances_vbo->binding_target, cell_instances_vbo->id);

    CellInstance moved_cell_instance;
    glGetBufferSubData(cell_instances_vbo->binding_target, cell_instance_position, cell_instances_vbo->element_size, &moved_cell_instance);

    glBindBuffer(cell_instances_vbo->binding_target, 0);

    Cell *moved_cell = get_cell(maze, moved_cell_instance.world_cell_position_x, moved_cell_instance.world_cell_position_y);
    if (moved_cell)
    {
      moved_cell->opengl_instance_position = cell_instance_position;
    }
    else
    {
      log(L_CellInstancing, "Could not get cell being moved from the Maze to update it's instance position");
    }
  }

  log(L_CellInstancing, "n_cell_instances: %u, out of cell_instances_vbo_size: %u", cell_instances_vbo->elements_used, cell_instances_vbo->total_elements);
}


void
draw_instanced_cells(CellInstancing *cell_instancing, Panning *panning, mat4 projection_matrix)
{
  glUseProgram(cell_instancing->shader_program);

  CellUniforms *uniforms = &cell_instancing->uniforms;
  glUniformMatrix4fv(uniforms->mat4_projection_matrix.location, 1, GL_TRUE, projection_matrix.es);

  glUniform1i(uniforms->int_render_origin_cell_x.location, panning->world_maze_pos.cell_x);
  glUniform1i(uniforms->int_render_origin_cell_y.location, panning->world_maze_pos.cell_y);
  glUniform2f(uniforms->vec2_render_origin_offset.location, panning->world_maze_pos.offset.x, panning->world_maze_pos.offset.y);
  glUniform1f(uniforms->float_scale.location, panning->zoom);

  glBindVertexArray(cell_instancing->vao);
  glDrawElementsInstanced(GL_TRIANGLES, cell_instancing->cell_vertex_ibo.elements_used, GL_UNSIGNED_SHORT, 0, cell_instancing->cell_instances_vbo.elements_used);
  glBindVertexArray(0);
}


// Setup functions


void
setup_cell_vertex_vbo_attributes(OpenGL_Buffer *cell_vertex_vbo)
{
  glBindBuffer(cell_vertex_vbo->binding_target, cell_vertex_vbo->id);

  GLuint attribute_vertex = 8;
  glVertexAttribPointer(attribute_vertex, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
  glEnableVertexAttribArray(attribute_vertex);
}


void
setup_cell_vertex_vbo(OpenGL_Buffer *cell_vertex_vbo, const vec2 *vertices, u32 n_vertices)
{
  cell_vertex_vbo->id = create_buffer();
  cell_vertex_vbo->element_size = sizeof(vec2);
  cell_vertex_vbo->total_elements = n_vertices;
  cell_vertex_vbo->elements_used = n_vertices;
  cell_vertex_vbo->binding_target = GL_ARRAY_BUFFER;
  cell_vertex_vbo->usage = GL_STATIC_DRAW;
  cell_vertex_vbo->setup_attributes_func = setup_cell_vertex_vbo_attributes;

  glBindBuffer(cell_vertex_vbo->binding_target, cell_vertex_vbo->id);
  glBufferData(cell_vertex_vbo->binding_target, cell_vertex_vbo->element_size * cell_vertex_vbo->elements_used, vertices, cell_vertex_vbo->usage);

  cell_vertex_vbo->setup_attributes_func(cell_vertex_vbo);
}


void
setup_cell_vertex_ibo_attributes(OpenGL_Buffer *cell_vertex_ibo)
{}


void
setup_cell_vertex_ibo(OpenGL_Buffer *cell_vertex_ibo, const GLushort *indices, u32 n_indices)
{
  cell_vertex_ibo->id = create_buffer();
  cell_vertex_ibo->element_size = sizeof(GLushort);
  cell_vertex_ibo->total_elements = n_indices;
  cell_vertex_ibo->elements_used = n_indices;
  cell_vertex_ibo->binding_target = GL_ELEMENT_ARRAY_BUFFER;
  cell_vertex_ibo->usage = GL_STATIC_DRAW;
  cell_vertex_ibo->setup_attributes_func = setup_cell_vertex_ibo_attributes;

  glBindBuffer(cell_vertex_ibo->binding_target, cell_vertex_ibo->id);
  glBufferData(cell_vertex_ibo->binding_target, cell_vertex_ibo->element_size * cell_vertex_ibo->elements_used, indices, cell_vertex_ibo->usage);

  cell_vertex_ibo->setup_attributes_func(cell_vertex_ibo);
}


void
setup_cell_instances_vbo_attributes(OpenGL_Buffer *cell_instances_vbo)
{
  glBindBuffer(cell_instances_vbo->binding_target, cell_instances_vbo->id);

  GLuint attribute_world_cell_position_x = 0;
  glEnableVertexAttribArray(attribute_world_cell_position_x);
  glVertexAttribIPointer(attribute_world_cell_position_x, 1, GL_INT, sizeof(CellInstance), (void *)offsetof(CellInstance, world_cell_position_x));
  glVertexAttribDivisor(attribute_world_cell_position_x, 1);

  GLuint attribute_world_cell_position_y = 1;
  glEnableVertexAttribArray(attribute_world_cell_position_y);
  glVertexAttribIPointer(attribute_world_cell_position_y, 1, GL_INT, sizeof(CellInstance), (void *)offsetof(CellInstance, world_cell_position_y));
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
setup_cell_instances_vbo(OpenGL_Buffer *cell_instances_vbo)
{
  cell_instances_vbo->id = create_buffer();
  cell_instances_vbo->element_size = sizeof(CellInstance);
  cell_instances_vbo->total_elements = 16;
  cell_instances_vbo->elements_used = 0;
  cell_instances_vbo->binding_target = GL_ARRAY_BUFFER;
  cell_instances_vbo->usage = GL_STATIC_DRAW;
  cell_instances_vbo->setup_attributes_func = setup_cell_instances_vbo_attributes;

  glBindBuffer(cell_instances_vbo->binding_target, cell_instances_vbo->id);
  glBufferData(cell_instances_vbo->binding_target, cell_instances_vbo->element_size * cell_instances_vbo->total_elements, 0, cell_instances_vbo->usage);

  cell_instances_vbo->setup_attributes_func(cell_instances_vbo);
}


b32
setup_cell_instancing(CellInstancing *cell_instancing)
{
  b32 success = true;

  const char *filenames[] = {"cell-instancing.glvs", "cell-instancing.glfs"};
  GLenum shader_types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

  success &= create_shader_program(filenames, shader_types, array_count(shader_types), &cell_instancing->shader_program);
  glUseProgram(cell_instancing->shader_program);

  cell_instancing->vao = create_vao();
  glBindVertexArray(cell_instancing->vao);

  setup_cell_vertex_vbo(&cell_instancing->cell_vertex_vbo, CELL_VERTICES,         array_count(CELL_VERTICES));
  setup_cell_vertex_ibo(&cell_instancing->cell_vertex_ibo, CELL_TRIANGLE_INDICES, array_count(CELL_TRIANGLE_INDICES));
  setup_cell_instances_vbo(&cell_instancing->cell_instances_vbo);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  CellUniforms *uniforms = &cell_instancing->uniforms;
  uniforms->mat4_projection_matrix.name = "projection_matrix";
  uniforms->int_render_origin_cell_x.name = "render_origin_cell_x";
  uniforms->int_render_origin_cell_y.name = "render_origin_cell_y";
  uniforms->vec2_render_origin_offset.name = "render_origin_offset";
  uniforms->float_scale.name = "scale";
  success &= get_uniform_locations(cell_instancing->shader_program, uniforms->array, array_count(uniforms->array));

  success &= print_gl_errors();
  log(L_CellInstancing, "OpenGL VBOs setup finished.");

  return success;
}