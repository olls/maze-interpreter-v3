const b32 FULLSCREEN = false;
const u32 WINDOW_WIDTH  = 1920*.5;
const u32 WINDOW_HEIGHT = 1080*.5;

const u32 FPS = 60;
const u32 TOTAL_MEMORY = megabytes_to_bytes(200);


struct Renderer
{
  SDL_Window *window;
  SDL_GLContext gl_context;

  u32 width;
  u32 height;
};


struct Key
{
  b32 down;
  b32 on_up;
  b32 on_down;
};

struct Keys
{
  b32 updated;

  Key up;
  Key down;
  Key left;
  Key right;
  Key backspace;
  Key enter;
  Key alpha_num_sym[MAX_CHAR - MIN_CHAR];
};


struct Mouse
{
  // TODO: Vector?
  u32 x;
  u32 y;

  b32 l_down;
  b32 r_down;
  b32 l_on_down;
  b32 r_on_down;
  b32 l_on_up;
  b32 r_on_up;

  vec2 scroll;
};


struct FPSCounter
{
  u32 frame_count;
  u64 last_update;
  u32 current_avg;
};


typedef b32 (*UpdateAndRenderFunc)(Memory *memory, Renderer *renderer, FT_Library *font_library,
                                   Keys *keys, Mouse *mouse, u64 time_us, u32 last_frame_dt, u32 fps,
                                   u32 argc, const u8 *argv[]);
