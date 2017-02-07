struct RenderBasis
{
  V2 origin;
  u32 world_per_pixel;
  r32 scale;
  V2 scale_focus;
  Rectangle clip_region;
};


struct FrameBuffer
{
  u32 width;
  u32 height;

  PixelColor *buffer;
};


struct Renderer
{
  SDL_Renderer *sdlRenderer;
  SDL_Texture * sdlTexture;

  FrameBuffer frame_buffer;
};


enum LineEndStyle
{
  LINE_END_NULL,

  LINE_END_BUTT,
  LINE_END_ROUND,
  LINE_END_SQUARE,
  LINE_END_MITER, // Unimplemented
  LINE_END_BEVEL // Unimplemented
};
