struct Uniforms
{
  union {
    Uniform array[4];

    struct {
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
  u32 n_cell_instances;
};


struct CellInstance
{
  s32 world_cell_position_x;
  s32 world_cell_position_y;
  V2 world_cell_offset;
  V4 colour;
};
