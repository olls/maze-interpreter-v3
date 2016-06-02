#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>

// Game Independent
#include "platform.h"
#include "maths.h"
#include "vectors.h"

// Game Related
#include "functions.h"
#include "maze.h"
#include "cars.h"
#include "maze-interpreter.h"
#include "render.h"
#include "parser.h"

#include "main.h"

#include "platform.cpp"
#include "maths.cpp"
#include "vectors.cpp"

#include "maze-interpreter.cpp"


const b32 FULLSCREEN = false;

const u32 FPS = 60;
const u32 TOTAL_MEMORY = megabytes_to_bytes(50);


u64
get_us()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return ((u64)tv.tv_sec * (u64)1000000) + (u64)tv.tv_usec;
}


b32
process_keys(Keys *keys, SDL_Event event)
{
  b32 quit = false;
  b32 pressed = (event.type == SDL_KEYDOWN);

  if ((pressed && event.key.keysym.sym == 'w' && event.key.keysym.mod == KMOD_LCTRL) ||
      (pressed && event.key.keysym.sym == SDLK_F4 && event.key.keysym.mod == KMOD_LALT))
  {
    quit = true;
  }
  else
  {

    switch (event.key.keysym.sym)
    {
    case ' ':
      keys->space = pressed;
      break;
    case SDLK_UP:
      keys->up = pressed;
      break;
    case SDLK_DOWN:
      keys->down = pressed;
      break;
    case SDLK_LEFT:
      keys->left = pressed;
      break;
    case SDLK_RIGHT:
      keys->right = pressed;
      break;
    case 'p':
      keys->p = pressed;
      break;
    case 'w':
      keys->w = pressed;
      break;
    case 'a':
      keys->a = pressed;
      break;
    case 's':
      keys->s = pressed;
      break;
    case 'd':
      keys->d = pressed;
      break;
    }
  }

  return quit;
}


void
process_mouse(Mouse * mouse, SDL_Event event)
{
  switch (event.type) {
    case SDL_MOUSEMOTION:
    {
      mouse->x = event.motion.x;
      mouse->y = event.motion.y;
    } break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    {
      b32 down = (event.type == SDL_MOUSEBUTTONDOWN);

      if (event.button.button == SDL_BUTTON_LEFT)
      {
        mouse->l_down = down;
      }
      else if (event.button.button == SDL_BUTTON_RIGHT)
      {
        mouse->r_down = down;
      }
    } break;

    case SDL_MOUSEWHEEL:
    {
      mouse->scroll.x -= event.wheel.x;
      mouse->scroll.y -= event.wheel.y;
    } break;
  }
}


void
game_loop(Memory * memory, Renderer * renderer, u32 argc, char * argv[])
{
  b32 running = true;

  u32 useconds_per_frame = 1000000 / FPS;

  GameState game_state = {};
  Keys keys = {};
  Mouse mouse = {};
  mouse.x = -1;
  mouse.y = -1;

  while (running)
  {
    u64 last_frame_end = get_us();

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
        {
          running = false;
        } break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
          running &= !process_keys(&keys, event);
        } break;

        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEWHEEL:
        {
          process_mouse(&mouse, event);
        } break;
      }
    }

    update_and_render(memory, &game_state, &(renderer->frame_buffer), &keys, &mouse, last_frame_end, argc, argv);

    SDL_UpdateTexture(renderer->sdlTexture, 0, renderer->frame_buffer.buffer, renderer->frame_buffer.width * sizeof(u32));
    SDL_RenderCopy(renderer->sdlRenderer, renderer->sdlTexture, 0, 0);
    SDL_RenderPresent(renderer->sdlRenderer);

    u32 frame_dt = get_us() - last_frame_end;

    if (frame_dt < useconds_per_frame)
    {
      usleep(useconds_per_frame - frame_dt);
    }
    else
    {
      printf("Missed frame rate: %d\n", frame_dt);
    }
  }
}


int
main(int argc, char * argv[])
{
  srand(time(0));

  Memory memory;
  memory.total = TOTAL_MEMORY;
  memory.memory = (u8 *)malloc(memory.total);
  memory.used = 0;

  Renderer renderer;

  SDL_Init(SDL_INIT_VIDEO);

  SDL_WindowFlags flags = (SDL_WindowFlags)(SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
  if (FULLSCREEN)
  {
    flags = (SDL_WindowFlags)(flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
  }
  else
  {
    renderer.frame_buffer.width = 1920;
    renderer.frame_buffer.height = 1080;
  }

  SDL_Window * window = SDL_CreateWindow("2D Game",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, renderer.frame_buffer.width, renderer.frame_buffer.height, flags);

  if (FULLSCREEN)
  {
    s32 display_index = SDL_GetWindowDisplayIndex(window);
    if (display_index < 0)
    {
      printf("Failed to get display index.\n");
      exit(1);
    }
    SDL_Rect window_rect;
    if (SDL_GetDisplayBounds(display_index, &window_rect))
    {
      printf("Failed to get display bounds.\n");
      exit(1);
    }
    renderer.frame_buffer.width = window_rect.w;
    renderer.frame_buffer.height = window_rect.h;
  }

  renderer.frame_buffer.buffer = push_structs(&memory, PixelColor, renderer.frame_buffer.width * renderer.frame_buffer.height);

  renderer.sdlRenderer = SDL_CreateRenderer(window, -1, 0);
  renderer.sdlTexture = SDL_CreateTexture(renderer.sdlRenderer,
    SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, renderer.frame_buffer.width, renderer.frame_buffer.height);

  game_loop(&memory, &renderer, argc, argv);

  SDL_DestroyTexture(renderer.sdlTexture);
  SDL_DestroyRenderer(renderer.sdlRenderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  free(memory.memory);

  return 0;
}