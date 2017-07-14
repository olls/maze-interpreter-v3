const u32 INITIAL_CELL_INSTANCE_VBO_SIZE = 16;
const u32 INVALID_CELL_INSTANCE_POSITION = MAX_U32;


struct Uniforms
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


struct OpenGL_VBOs
{
  GLuint cell_vertex_vbo;
  GLuint cell_vertex_ibo;
  u32 n_cell_indices;

  GLuint cell_instances_vbo;
  u32 cell_instances_vbo_size;
  u32 n_cell_instances;
};


struct CellInstance
{
  s32 world_cell_position_x;
  s32 world_cell_position_y;
  vec2 world_cell_offset;
  vec4 colour;
};
