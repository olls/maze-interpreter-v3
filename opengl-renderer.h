struct Renderer
{
  SDL_Window *window;
  SDL_GLContext gl_context;

  u32 width;
  u32 height;
};


typedef GLint OpenGLViewport[4];


struct OpenGLProjection
{
  GLdouble modelview[16];
  GLdouble projection[16];
};
