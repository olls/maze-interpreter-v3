#include <sys/time.h>

#include <time.h>
#include <SDL2/SDL.h>

#include "thingy.h"


const uint32_t FPS = 30;

const uint32_t WINDOW_WIDTH = 1024;
const uint32_t WINDOW_HEIGHT = 600;

// NOTE: 256 sub-pixel steps!
const float WORLD_COORD_TO_PIXELS = 1.0f / 256.0f;

const uint32_t CELL_GRID_WIDTH = 15;
const uint32_t CELL_GRID_HEIGHT = 15;

const uint32_t CELL_SPACING = 10000;
const uint32_t CELL_MARGIN = 1000;

const uint32_t CAR_SIZE = 6000;


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


Cell *
get_cell(Cell * cells, uint32_t cell_x, uint32_t cell_y)
{
  return cells + (cell_y * CELL_GRID_WIDTH) + cell_x;
}


void
render_cells(uint32_t * pixels, Mouse mouse, Cell * cells)
{
  for (uint32_t cell_y = 0;
       cell_y < CELL_GRID_HEIGHT;
       cell_y++)
  {
    for (uint32_t cell_x = 0;
         cell_x < CELL_GRID_WIDTH;
         cell_x++)
    {
      Cell * cell = get_cell(cells, cell_x, cell_y);

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

      // Into the pixel space!
      uint32_t x = (cell_x * CELL_SPACING) * WORLD_COORD_TO_PIXELS;
      uint32_t y = (cell_y * CELL_SPACING) * WORLD_COORD_TO_PIXELS;
      uint32_t width = (CELL_SPACING - CELL_MARGIN) * WORLD_COORD_TO_PIXELS;
      uint32_t height = (CELL_SPACING - CELL_MARGIN) * WORLD_COORD_TO_PIXELS;

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


Car *
add_car(Cars * cars, uint32_t x, uint32_t y)
{
  // TODO: Change car storage to make removal quicker.

  assert(cars->first_free < array_count(cars->cars));
  Car * car = cars->cars + (cars->first_free++);
  car->exists = true;
  car->x = x;
  car->y = y;
  car->direction = UP;

  return car;
}

void
rm_car(Cars * cars, uint32_t car_index)
{
  // Move all cars after car_index down one
  for (uint32_t index = car_index;
       index < (array_count(cars->cars) - 1);
       ++index)
  {
    cars->cars[index] = cars->cars[index + 1];
  }

  --cars->first_free;
}


void
update_cars(uint32_t * pixels, uint32_t df, uint32_t frame_count, Keys keys, Mouse mouse,
            Cell * cells, Cars * cars)
{

  uint32_t car_index = 0;
  Car * car = cars->cars + car_index;

  while (car->exists)
  {

    // TODO: Animation
    // float delta_move_per_s = CELL_SPACING;
    // float delta_move_per_us = delta_move_per_s / seconds_in_u(1);
    // float delta_move_per_frame = delta_move_per_us * df;
    // car->x += delta_move_per_frame;

    // Cell actions
    uint32_t cell_x = car->x / CELL_SPACING;
    uint32_t cell_y = car->y / CELL_SPACING;

    Cell * current_cell = get_cell(cells, cell_x, cell_y);

    // TODO: Deal with race cars (conditions)

    switch (current_cell->type)
    {  
      case (CELL_NULL):
      {
        printf("Null\n");
      } break;
  
      case (CELL_START):
      {
        printf("Start\n");
      } break;
  
      case (CELL_PATH):
      {
      } break;
  
      case (CELL_WALL):
      {
        printf("Wall\n");
        invalid_code_path;
      } break;
  
      case (CELL_HOLE):
      {
        printf("Hole\n");
        rm_car(cars, car_index);
      } break;
  
      case (CELL_SPLITTER):
      {
        printf("Splitter\n");
        add_car(cars, car->x, car->y);
      } break;
  
      case (CELL_FUNCTION):
      {
        printf("Function\n");
      } break;
  
      case (CELL_ONCE):
      {
        printf("Once\n");
      } break;

      default:
      {
        invalid_code_path;
      } break;
    }

    // Car movements
    if (car->direction == STATIONARY)
    {
      // Leave them alone!
    }
    else
    {

      Direction directions[4];

      directions[0] = car->direction;
      directions[3] = reverse(car->direction);

      uint32_t i = 1;
      for (uint32_t direction = 0;
           direction < 4;
           direction++)
      {
        if ((direction != directions[0]) &&
            (direction != directions[3]))
        {
          directions[i++] = (Direction)direction;
        }
      }

      bool walls[4];
      walls[UP]    = get_cell(cells, cell_x, (cell_y + 1))->type == CELL_WALL;
      walls[DOWN]  = get_cell(cells, cell_x, (cell_y - 1))->type == CELL_WALL;
      walls[LEFT]  = get_cell(cells, (cell_x - 1), cell_y)->type == CELL_WALL;
      walls[RIGHT] = get_cell(cells, (cell_x + 1), cell_y)->type == CELL_WALL;

      Direction test_direction;
      bool can_move = false;
      for (uint32_t direction_index = 0;
           direction_index < 4;
           direction_index++)
      {
        test_direction = directions[direction_index];
        if (!walls[test_direction])
        {
          can_move = true;
          car->direction = test_direction;
          break;
        }
      }

      if (can_move)
      {
        switch (car->direction)
        {
          case UP:
          {
            car->y += CELL_SPACING;
          } break;

          case DOWN:
          {
            car->y -= CELL_SPACING;
          } break;

          case LEFT:
          {
            car->x -= CELL_SPACING;
          } break;

          case RIGHT:
          {
            car->x += CELL_SPACING;
          } break;

          case STATIONARY:
          {
          } break;

          default:
          {
            invalid_code_path;
          }
        }
      }
    }

    car = cars->cars + (++car_index);
  }
}


void
render_cars(uint32_t * pixels, uint32_t df, Cars * cars)
{
  uint32_t car_index = 0;
  Car * car = cars->cars + car_index;

  while (car->exists)
  {
    // Render
    uint32_t test = ((CELL_SPACING - CELL_MARGIN - CAR_SIZE) * 0.5f);
    uint32_t x = (uint32_t)(WORLD_COORD_TO_PIXELS * (car->x + ((CELL_SPACING - CELL_MARGIN - CAR_SIZE) * 0.5f)));
    uint32_t y = (uint32_t)(WORLD_COORD_TO_PIXELS * (car->y + ((CELL_SPACING - CELL_MARGIN - CAR_SIZE) * 0.5f)));
    uint32_t size = (uint32_t)(WORLD_COORD_TO_PIXELS * CAR_SIZE);

    draw_box(pixels, x, y, size, size, 0x00992277);

    car = cars->cars + (++car_index);
  }
}


void
update_and_render(GameMemory * game_memory, uint32_t * pixels, uint32_t df, uint32_t frame_count,
                  Keys keys, Mouse mouse, Cell * cells, Cars * cars)
{
  render_cells(pixels, mouse, cells);

  if (frame_count % 2 == 0)
  {
    update_cars(pixels, df, frame_count, keys, mouse, cells, cars);
  }

  render_cars(pixels, df, cars);
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

  // The car list
  Cars * cars = (Cars *)take_mem(&game_memory, sizeof(Cars));
  init_car_mem(cars);

  // Init cells and cars
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
      else if ((cell_x == 1 && cell_y == 1) ||
               (cell_x == 9 && cell_y == 9))
      {
        cell->type = CELL_START;
        add_car(cars, cell_x * CELL_SPACING, cell_y * CELL_SPACING);
      }
      else if ((cell_x == 2 && cell_y == 1) ||
               (cell_x == 7 && cell_y == 1) ||
               (cell_x == 3 && cell_y == 2))
      {
        cell->type = CELL_WALL;
      }
      else if (cell_x == 8 && cell_y == 7)
      {
        cell->type = CELL_HOLE;
      }
      else if (cell_x == 8 && cell_y == 5)
      {
        cell->type = CELL_SPLITTER;
      }
      else
      {
        cell->type = CELL_PATH;
      }
    }
  }

  // Initialise keys
  Keys keys;
  keys.space_down = false;
  keys.up_down = false;
  keys.down_down = false;
  keys.left_down = false;
  keys.right_down = false;
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

      switch (event.type)
      {
        case SDL_QUIT:
        {
          quit = true;
        } break;

        case SDL_KEYDOWN:
        {
          SDL_Keycode key = event.key.keysym.sym;

          if (key == 'w' && event.key.keysym.mod == KMOD_LCTRL)
          {
            quit = true;
          }

          switch (key)
          {
          case ' ':
            keys.space_down = true;
            break;
          case SDLK_UP:
            keys.up_down = true;
            break;
          case SDLK_DOWN:
            keys.down_down = true;
            break;
          case SDLK_LEFT:
            keys.left_down = true;
            break;
          case SDLK_RIGHT:
            keys.right_down = true;
            break;
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
        } break;

        case SDL_KEYUP:
        {
          SDL_Keycode key = event.key.keysym.sym;

          switch (key)
          {
          case ' ':
            keys.space_down = false;
            break;
          case SDLK_UP:
            keys.up_down = false;
            break;
          case SDLK_DOWN:
            keys.down_down = false;
            break;
          case SDLK_LEFT:
            keys.left_down = false;
            break;
          case SDLK_RIGHT:
            keys.right_down = false;
            break;
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
        } break;

        case SDL_MOUSEMOTION:
        {
          // NOTE: Remember our Y is inverted from SDL
          mouse.x = event.motion.x;
          mouse.y = WINDOW_HEIGHT - event.motion.y;
        } break;

        case SDL_MOUSEBUTTONDOWN:
        {
          switch (event.button.button)
          {
            case SDL_BUTTON_LEFT:
              mouse.l_down = true;
              break;
            case SDL_BUTTON_RIGHT:
              mouse.r_down = true;
              break;
          }
        } break;

        case SDL_MOUSEBUTTONUP:
        {
          switch (event.button.button)
          {
            case SDL_BUTTON_LEFT:
              mouse.l_down = false;
              break;
            case SDL_BUTTON_RIGHT:
              mouse.r_down = false;
              break;
          }
        } break;
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

      update_and_render(&game_memory, pixels, delta_frame, frame_count, keys, mouse, cells, cars);

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
