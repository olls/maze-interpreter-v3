const u32 INITIAL_GL_BUFFER_TOTAL_ELEMENTS = 16;
const u32 INVALID_GL_BUFFER_ELEMENT_POSITION = MAX_U32;


struct Uniform
{
  const u8 *name;
  GLuint location;
};


struct OpenGL_Buffer
{
  GLuint id;

  u32 element_size;
  u32 total_elements;
  u32 elements_used;

  GLenum binding_target;
  GLenum usage;

  void (*setup_attributes_func)(OpenGL_Buffer *);
};


struct VertexBuffer
{
  vec2 *vertices;
  u32 n_vertices;
};