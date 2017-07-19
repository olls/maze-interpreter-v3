struct CellUniforms
{
  union {
    Uniform array[5];

    struct {
      Uniform mat4_projection_matrix;
      Uniform int_render_origin_cell_x;
      Uniform int_render_origin_cell_y;
      Uniform vec2_render_origin_offset;
      Uniform float_scale;
    };
  };
};


struct CellInstancing
{
  GLuint shader_program;
  CellUniforms uniforms;

  GLuint vao;

  OpenGL_Buffer cell_vertex_vbo;
  OpenGL_Buffer cell_vertex_ibo;
  OpenGL_Buffer cell_instances_vbo;
};


struct CellInstance
{
  s32 world_cell_position_x;
  s32 world_cell_position_y;
  vec2 world_cell_offset;
  vec4 colour;
};


const vec2 CELL_VERTICES[] = {
  {0.3, 0.0},
  {0.7, 0.0},
  {0.7, 0.3},
  {1.0, 0.3},
  {1.0, 0.7},
  {0.7, 0.7},
  {0.7, 1.0},
  {0.3, 1.0},
  {0.3, 0.7},
  {0.0, 0.7},
  {0.0, 0.3},
  {0.3, 0.3}
};

const GLushort CELL_TRIANGLE_INDICES[] = {
  11, 0,  1, // Top
  11, 1,  2,
  2,  3,  4,  // Right
  2,  4,  5,
  5,  6,  7,  // Bottom
  5,  7,  8,
  8,  9,  10, // Left
  8,  10, 11,
  11, 2,  5, // Middle
  11, 5,  8
};