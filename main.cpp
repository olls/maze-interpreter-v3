#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <SDL2/SDL.h>

// Game Independent
#include "platform.h"
#include "maths.h"
#include "vectors.h"
#include "render.h"
#include "bitmap.h"
#include "font.h"

#include "main.h"

#include "logging.cpp"
#include "platform.cpp"
#include "maths.cpp"
#include "vectors.cpp"
#include "render.cpp"
#include "colors.cpp"
#include "bitmap.cpp"
#include "font.cpp"

// Game Related
#include "functions.h"
#include "particles.h"
#include "cells-storage.h"
#include "cars-storage.h"
#include "cars.h"
#include "cells.h"
#include "parser.h"
#include "serialize.h"
#include "draw.h"
#include "ui.h"
#include "input.h"

#include "maze-interpreter.h"
#include "render-queue.h"

#include "functions.cpp"
#include "particles.cpp"
#include "cells-storage.cpp"
#include "cars-storage.cpp"
#include "cars.cpp"
#include "cells.cpp"
#include "parser.cpp"
#include "serialize.cpp"
#include "draw.cpp"
#include "ui.cpp"
#include "input.cpp"
#include "render-queue.cpp"

#include "maze-interpreter.cpp"


u64
get_us()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return ((u64)tv.tv_sec * (u64)1000000) + (u64)tv.tv_usec;
}


void
set_keys(Keys *keys)
{
  keys->updated = false;

  keys->space.on_up = false;
  keys->down.on_up = false;
  keys->left.on_up = false;
  keys->right.on_up = false;
  keys->minus.on_up = false;
  keys->equals.on_up = false;

  keys->space.on_down = false;
  keys->down.on_down = false;
  keys->left.on_down = false;
  keys->right.on_down = false;
  keys->minus.on_down = false;
  keys->equals.on_down = false;

  for (u32 i = 0; i < array_count(keys->alpha); ++i)
  {
    keys->alpha[i].on_up = false;
    keys->alpha[i].on_down = false;
  }
}


void
set_mouse(Mouse *mouse)
{
  mouse->l_on_down = false;
  mouse->r_on_down = false;
  mouse->l_on_up = false;
  mouse->r_on_up = false;
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
    Key *input = 0;
    SDL_Keysym key = event.key.keysym;
    if (key.sym >= 'a' && key.sym <= 'z')
    {
      input = keys->alpha + (key.sym - 'a');
    }
    else
    {
      switch (key.sym)
      {
      case ' ':
        input = &keys->space;
        break;
      case SDLK_UP:
        input = &keys->up;
        break;
      case SDLK_DOWN:
        input = &keys->down;
        break;
      case SDLK_LEFT:
        input = &keys->left;
        break;
      case SDLK_RIGHT:
        input = &keys->right;
        break;
      case '-':
        input = &keys->minus;
        break;
      case '=':
        input = &keys->equals;
        break;
      }
    }

    if (input)
    {
      input->on_up = !pressed && input->down;
      input->on_down = pressed && !input->down;

      keys->updated |= input->on_up || input->on_down;

      input->down = pressed;
    }
  }

  return quit;
}


void
process_mouse(Mouse *mouse, SDL_Event event)
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
        mouse->l_on_down = down;
        mouse->l_on_up = !down;
      }
      else if (event.button.button == SDL_BUTTON_RIGHT)
      {
        mouse->r_down = down;
        mouse->l_on_down = down;
        mouse->l_on_up = !down;
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
game_loop(Memory *memory, Renderer *renderer, u32 argc, char *argv[])
{
  b32 running = true;

  u32 useconds_per_frame = 1000000 / FPS;
  u32 frame_dt = useconds_per_frame;

  FPSCounter fps = {
    .frame_count = 0,
    .last_update = get_us()
  };

  GameState game_state = {};
  Keys keys = {};
  Mouse mouse = {};
  mouse.x = -1;
  mouse.y = -1;

  while (running)
  {
    u64 last_frame_end = get_us();

    mouse.scroll -= mouse.scroll / 6.0f;
    r32 epsilon = 1.5f;
    if (abs(mouse.scroll.y) < epsilon)
    {
      mouse.scroll.y = 0;
    }
    if (abs(mouse.scroll.x) < epsilon)
    {
      mouse.scroll.x = 0;
    }

    set_keys(&keys);
    set_mouse(&mouse);

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

    update_and_render(memory, &game_state, &(renderer->frame_buffer), &keys, &mouse, last_frame_end, frame_dt, fps.current_avg, argc, argv);

    SDL_UpdateTexture(renderer->sdlTexture, 0, renderer->frame_buffer.buffer, renderer->frame_buffer.width * sizeof(u32));
    SDL_RenderCopy(renderer->sdlRenderer, renderer->sdlTexture, 0, 0);
    SDL_RenderPresent(renderer->sdlRenderer);

    ++fps.frame_count;
    if (last_frame_end >= fps.last_update + seconds_in_u(1))
    {
      fps.last_update = last_frame_end;
      fps.current_avg = fps.frame_count;
      fps.frame_count = 0;
    }

    frame_dt = get_us() - last_frame_end;

    if (frame_dt < useconds_per_frame)
    {
      usleep(useconds_per_frame - frame_dt);
    }
    else
    {
      log(L_Main, "Missed frame rate: %d", frame_dt);
    }
  }
}


int
main(int argc, char *argv[])
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
    renderer.frame_buffer.width = WINDOW_WIDTH;
    renderer.frame_buffer.height = WINDOW_HEIGHT;
  }

  SDL_Window *window = SDL_CreateWindow("Maze Interpreter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, renderer.frame_buffer.width, renderer.frame_buffer.height, flags);

  if (FULLSCREEN)
  {
    s32 display_index = SDL_GetWindowDisplayIndex(window);
    if (display_index < 0)
    {
      log(L_Main, "Failed to get display index.");
      exit(1);
    }
    SDL_Rect window_rect;
    if (SDL_GetDisplayBounds(display_index, &window_rect))
    {
      log(L_Main, "Failed to get display bounds.");
      exit(1);
    }
    renderer.frame_buffer.width = window_rect.w;
    renderer.frame_buffer.height = window_rect.h;
  }

  renderer.frame_buffer.buffer = push_structs(&memory, PixelColor, renderer.frame_buffer.width * renderer.frame_buffer.height);

  renderer.sdlRenderer = SDL_CreateRenderer(window, -1, 0);
  renderer.sdlTexture = SDL_CreateTexture(renderer.sdlRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, renderer.frame_buffer.width, renderer.frame_buffer.height);

  game_loop(&memory, &renderer, argc, argv);

  SDL_DestroyTexture(renderer.sdlTexture);
  SDL_DestroyRenderer(renderer.sdlRenderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  free(memory.memory);

  return 0;
}