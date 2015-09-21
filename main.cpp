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

const uint32_t MAX_MEM = megabytes(5);

const uint32_t WINDOW_WIDTH = 1024;
const uint32_t WINDOW_HEIGHT = 600;


uint64_t
get_us()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((uint64_t)tv.tv_sec * (uint64_t)1000000) + (uint64_t)tv.tv_usec;
}


void
assert(bool cond)
{
  if (!cond)
  {
    printf("Assertion FAILED!! :(\n");
    exit(1);
  }
}


// NOTE: size is in bytes.
void *
take_mem(GameMemory * game_memory, uint32_t size)
{
  void * result = (void *)game_memory->pos;

  game_memory->pos = game_memory->pos + size;
  assert(game_memory->pos < game_memory->memory + game_memory->total);

  return result;
}


void
draw_box(uint32_t * pixels,
         uint32_t start_x,
         uint32_t start_y,
         uint32_t width,
         uint32_t height)
{
  uint32_t end_x = start_x + width;
  uint32_t end_y = start_y + height;

  for (uint32_t pixel_y = start_y;
       pixel_y < end_y;
       pixel_y++)
  {
    for (uint32_t pixel_x = start_x;
         pixel_x < end_x;
         pixel_x++)
    {
      pixels[pixel_y * WINDOW_WIDTH + pixel_x] = 0;
    }
  }
}


void
draw_player(uint32_t * pixels, Player * player)
{
  draw_box(pixels, player->x, player->y, 30, 30);
}


void
update_and_render(GameMemory * game_memory, uint32_t * pixels, Keys keys, Player * player)
{
  int32_t dy = 0;
  int32_t dx = 0;

  if (keys.w_down)
  {
    dy++;
  }
  if (keys.s_down)
  {
    dy--;
  }
  if (keys.d_down)
  {
    dx++;
  }
  if (keys.a_down)
  {
    dx--;
  }

  player->x += dx;
  player->y += dy;

  draw_player(pixels, player);
}


int
main(int32_t argc, char * argv[])
{
  srand(time(NULL));

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window * window = SDL_CreateWindow("A Thingy",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture * texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WINDOW_WIDTH, WINDOW_HEIGHT);


  GameMemory game_memory;

  game_memory.total = MAX_MEM;
  game_memory.memory = (uint8_t *)malloc(game_memory.total);
  game_memory.pos = game_memory.memory;
  assert(game_memory.memory != NULL);


  // The pixel buffer
  uint32_t * pixels = (uint32_t *)take_mem(&game_memory, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));

  Player * player = (Player *)take_mem(&game_memory, sizeof(Player));


  // Initalise keys
  Keys keys;
  keys.w_down = false;
  keys.a_down = false;
  keys.s_down = false;
  keys.d_down = false;

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

        if (key == 'w')
        {
          keys.w_down = true;
        }
        if (key == 'a')
        {
          keys.a_down = true;
        }
        if (key == 's')
        {
          keys.s_down = true;
        }
        if (key == 'd')
        {
          keys.d_down = true;
        }
        break;

      case SDL_KEYUP:

        if (key == 'w')
        {
          keys.w_down = false;
        }
        if (key == 'a')
        {
          keys.a_down = false;
        }
        if (key == 's')
        {
          keys.s_down = false;
        }
        if (key == 'd')
        {
          keys.d_down = false;
        }
        break;

      default:
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
    if (now >= last_frame + (1000000/FPS))
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
             screen_x < WINDOW_WIDTH  ;
             screen_x++)
        {
          pixels[screen_y * WINDOW_WIDTH + screen_x] = 0x00FFFFFF;
        }
      }

      update_and_render(&game_memory, pixels, keys, player);

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
