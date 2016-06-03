struct RenderBasis
{
  V2 origin;
  u32 scale;
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
