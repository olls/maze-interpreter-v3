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
