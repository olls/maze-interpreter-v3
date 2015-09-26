#include <sys/time.h>

#include <time.h>
#include <SDL2/SDL.h>

#include "thingy.h"


#define kilobytes(n) (1024 * n)
#define megabytes(n) (kilobytes(1024) * n)
#define gigabytes(n) (megabytes(1024) * n)

#define seconds_in_m(n) (1000 * n )
#define seconds_in_u(n) (seconds_in_m(1000) * n )

#define m_in_seconds(n) (n / (real32)seconds_in_m(1))
#define u_in_seconds(n) (n / (real32)seconds_in_u(1))

#define FPS 30

const uint32_t WINDOW_WIDTH = 1024;
const uint32_t WINDOW_HEIGHT = 600;

const uint32_t CELL_GRID_WIDTH = 15;
const uint32_t CELL_GRID_HEIGHT = 15;

const uint32_t MAX_CARS = 128;


uint64_t
get_us()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((uint64_t)tv.tv_sec * (uint64_t)seconds_in_u(1)) + (uint64_t)tv.tv_usec;
}


void
draw_box(uint32_t * pixels,
         int32_t start_x,
         int32_t start_y,
         uint32_t width,
         uint32_t height,
         uint32_t color)
{
  int32_t end_x = start_x + width;
  int32_t end_y = start_y + height;

  start_y = fmin(start_y, WINDOW_HEIGHT);
  start_x = fmin(start_x, WINDOW_WIDTH);
  end_y =   fmin(end_y,   WINDOW_HEIGHT);
  end_x =   fmin(end_x,   WINDOW_WIDTH);

  start_y = fmax(start_y, 0);
  start_x = fmax(start_x, 0);
  end_y =   fmax(end_y,   0);
  end_x =   fmax(end_x,   0);

  for (uint32_t pixel_y = start_y;
       pixel_y < end_y;
       pixel_y++)
  {
    for (uint32_t pixel_x = start_x;
         pixel_x < end_x;
         pixel_x++)
    {
      pixels[pixel_y * WINDOW_WIDTH + pixel_x] = color;
    }
  }
}


void
update_and_render(GameMemory * game_memory, uint32_t * pixels, Keys keys, Mouse mouse, Cell * cells)
{
  for (uint32_t cell_y = 0;
       cell_y < CELL_GRID_HEIGHT;
       cell_y++)
  {
    for (uint32_t cell_x = 0;
         cell_x < CELL_GRID_WIDTH;
         cell_x++)
    {
      Cell * cell = cells + cell_y * CELL_GRID_WIDTH + cell_x;

      uint32_t margin = 3;
      uint32_t x = cell_x * 30 + margin;
      uint32_t y = cell_y * 30 + margin;
      uint32_t width = 30 - margin * 2;
      uint32_t height = 30 - margin * 2;

      uint32_t color = 0;
      switch (cell->type)
      {
        case CELL_NULL:
          color = 0;
          break;
        case CELL_START:
          color = 0x0033AA55;
          break;
        case CELL_PATH:
          color = 0x00558822;
          break;
        case CELL_WALL:
          color = 0x00333333;
          break;
        case CELL_HOLE:
          color = 0x00BB6644;
          break;
        case CELL_SPLITTER:
          color = 0x00224499;
          break;
        case CELL_FUNCTION:
          color = 0x00667788;
          break;
        case CELL_ONCE:
          color = 0x00887766;
          break;
      }

      if ((mouse.x >= x) &&
          (mouse.x < x + width) &&
          (mouse.y >= y) &&
          (mouse.y < y + height))
      {
        uint32_t color_a = ((color >> 24) & 0xFF);
        uint32_t color_r = ((color >> 16) & 0xFF);
        uint32_t color_g = ((color >> 8) & 0xFF);
        uint32_t color_b = ((color >> 0) & 0xFF);

        if (color_r <= (0xFF - 0x20))
        {
          color_r += 0x20;
        }
        if (color_g <= (0xFF - 0x20))
        {
          color_g += 0x20;
        }
        if (color_b <= (0xFF - 0x20))
        {
          color_b += 0x20;
        }

        color = (color_a << 24) | (color_r << 16) | (color_g << 8) | (color_b << 0);
      }

      draw_box(pixels,
               x, y,
               width, height,
               color);
    }
  }
}


int
main(int32_t argc, char * argv[])
{
  srand(time(NULL));

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("A Maze Thingy",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture * texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WINDOW_WIDTH, WINDOW_HEIGHT);

  // Init game memory
  GameMemory game_memory;
  init_mem(&game_memory, megabytes(5));

  // The pixel buffer
  uint32_t * pixels = (uint32_t *)take_mem(&game_memory, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));

  // The cell grid
  Cell * cells = (Cell *)take_mem(&game_memory, CELL_GRID_WIDTH * CELL_GRID_HEIGHT * sizeof(Cell));

  for (uint32_t cell_y = 0;
       cell_y < CELL_GRID_HEIGHT;
       cell_y++)
  {
    for (uint32_t cell_x = 0;
         cell_x < CELL_GRID_WIDTH;
         cell_x++)
    {
      Cell * cell = cells + cell_y * CELL_GRID_WIDTH + cell_x;
      cell->data = 0;

      if (cell_x == 0 ||
          cell_x == (CELL_GRID_WIDTH - 1) ||
          cell_y == 0 ||
          cell_y == (CELL_GRID_HEIGHT - 1))
      {
        cell->type = CELL_WALL;
      }
      else
      {
        // cell->type = CELL_PATH;
        cell->type = (enum CellType)(cell_x % 8);
      }
    }
  }

  // The car list
  Car * cars = (Car *)take_sruct_mem(&game_memory, MAX_CARS, Car);

  // Initalise keys
  Keys keys;
  keys.p_down = false;
  keys.w_down = false;
  keys.a_down = false;
  keys.s_down = false;
  keys.d_down = false;

  // Initalise mouse
  Mouse mouse;
  mouse.x = -1;
  mouse.y = -1;
  mouse.l_down = false;
  mouse.r_down = false;

  // For average FPS measurement
  uint64_t last_measure = get_us();
  uint32_t frame_count = 0;

  // For FPS timing
  uint64_t last_frame = get_us();
  uint32_t delta_frame;

  printf("Starting\n");

  bool quit = false;
  while (!quit)
  {
    uint64_t now = get_us();

    // Get inputs
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      char key = event.key.keysym.sym;
      switch (event.type)
      {
      case SDL_QUIT:
        quit = true;
        break;

      case SDL_KEYDOWN:

        if (key == 'w' && event.key.keysym.mod == KMOD_LCTRL)
        {
          quit = true;
        }

        switch (key)
        {
        case 'p':
          keys.p_down = true;
          break;
        case 'w':
          keys.w_down = true;
          break;
        case 'a':
          keys.a_down = true;
          break;
        case 's':
          keys.s_down = true;
          break;
        case 'd':
          keys.d_down = true;
          break;
        }
        break;

      case SDL_KEYUP:
        switch (key)
        {
        case 'p':
          keys.p_down = false;
          break;
        case 'w':
          keys.w_down = false;
          break;
        case 'a':
          keys.a_down = false;
          break;
        case 's':
          keys.s_down = false;
          break;
        case 'd':
          keys.d_down = false;
          break;
        }
        break;

      case SDL_MOUSEMOTION:
        // NOTE: Remember our Y is inverted from SDL
        mouse.x = event.motion.x;
        mouse.y = WINDOW_HEIGHT - event.motion.y;
        break;

      case SDL_MOUSEBUTTONDOWN:
        switch (event.button.button)
        {
          case SDL_BUTTON_LEFT:
            mouse.l_down = true;
            break;
          case SDL_BUTTON_RIGHT:
            mouse.r_down = true;
            break;
        }
        break;

      case SDL_MOUSEBUTTONUP:
        switch (event.button.button)
        {
          case SDL_BUTTON_LEFT:
            mouse.l_down = false;
            break;
          case SDL_BUTTON_RIGHT:
            mouse.r_down = false;
            break;
        }
        break;
      }
    }

    // Measure FPS
    if (now >= (last_measure + seconds_in_u(1)))
    {
      // If last measurement was more than 1 sec ago
      last_measure = now;
      printf("%f us/frame\n", seconds_in_u(1) / (double)frame_count);
      frame_count = 0;
    }

    // Render
    if (now >= last_frame + (seconds_in_u(1)/FPS))
    {
      delta_frame = now - last_frame;
      last_frame = now;
      frame_count++;

      // Clear screen to white

      for (uint32_t screen_y = 0;
           screen_y < WINDOW_HEIGHT;
           screen_y++)
      {
        for (uint32_t screen_x = 0;
             screen_x < WINDOW_WIDTH;
             screen_x++)
        {
          pixels[screen_y * WINDOW_WIDTH + screen_x] = 0x00FFFFFF;
        }
      }

      update_and_render(&game_memory, pixels, keys, mouse, cells);

      // Flip pixels
      for (uint32_t pixel_y = 0;
           pixel_y < WINDOW_HEIGHT / 2;
           pixel_y++)
      {
        for (uint32_t pixel_x = 0;
             pixel_x < WINDOW_WIDTH;
             pixel_x++)
        {
          uint32_t top_pixel_pos = pixel_y * WINDOW_WIDTH + pixel_x;
          uint32_t top_pixel = pixels[top_pixel_pos];

          uint32_t bottom_pixel_pos = (WINDOW_HEIGHT - pixel_y - 1) * WINDOW_WIDTH + pixel_x;

          pixels[top_pixel_pos] = pixels[bottom_pixel_pos];
          pixels[bottom_pixel_pos] = top_pixel;
        }
      }

      SDL_UpdateTexture(texture, NULL, pixels, WINDOW_WIDTH * sizeof(uint32_t));

      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);

    }
  }

  printf("Quitting\n");

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  free(game_memory.memory);

  printf("Finished\n");

  return 0;
}
