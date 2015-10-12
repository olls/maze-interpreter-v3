#include <sys/time.h>

#include <time.h>
#include <SDL2/SDL.h>

#include "maze-interpreter.h"
#include "blocks.cpp"
#include "parser.cpp"


const uint32_t FPS = 30;
const uint32_t TOTAL_MEMORY = megabytes_to_bytes(50);

const uint32_t WINDOW_WIDTH = 1024;
const uint32_t WINDOW_HEIGHT = 600;

// NOTE: 256 sub-pixel steps!
const uint32_t PIXELS_TO_WORLD_COORDS = 256;
const float WORLD_COORDS_TO_PIXELS = 1.0f / (float)PIXELS_TO_WORLD_COORDS;

const uint32_t CELL_SPACING = 10000;
const uint32_t CELL_MARGIN = 1000;

const uint32_t CAR_SIZE = 6000;

const char MAZE_FILENAME[] = "test.mz";


uint64_t
get_us()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((uint64_t)tv.tv_sec * (uint64_t)seconds_in_u(1)) + (uint64_t)tv.tv_usec;
}


void
draw_box(uint32_t * pixels,
         int32_t start_x_world,
         int32_t start_y_world,
         uint32_t width_world,
         uint32_t height_world,
         V4 color)
{
  int32_t end_x_world = start_x_world + width_world;
  int32_t end_y_world = start_y_world + height_world;

  // Into the (floating point) pixel space!
  float start_x = start_x_world * WORLD_COORDS_TO_PIXELS;
  float start_y = start_y_world * WORLD_COORDS_TO_PIXELS;
  float end_x   = end_x_world * WORLD_COORDS_TO_PIXELS;
  float end_y   = end_y_world * WORLD_COORDS_TO_PIXELS;

  // Into the (rounded exact) pixel space!
  int32_t start_x_pixels = (int32_t)start_x;
  int32_t start_y_pixels = (int32_t)start_y;
  int32_t end_x_pixels   = (int32_t)end_x;
  int32_t end_y_pixels   = (int32_t)end_y;

  start_x_pixels = fmin(start_x_pixels, WINDOW_WIDTH - 1);
  start_y_pixels = fmin(start_y_pixels, WINDOW_HEIGHT - 1);
  end_x_pixels   = fmin(end_x_pixels,   WINDOW_WIDTH - 1);
  end_y_pixels   = fmin(end_y_pixels,   WINDOW_HEIGHT - 1);

  start_y_pixels = fmax(start_y_pixels, 0);
  start_x_pixels = fmax(start_x_pixels, 0);
  end_y_pixels   = fmax(end_y_pixels,   0);
  end_x_pixels   = fmax(end_x_pixels,   0);

  for (uint32_t pixel_y = start_y_pixels;
       pixel_y <= end_y_pixels;
       pixel_y++)
  {
    for (uint32_t pixel_x = start_x_pixels;
         pixel_x <= end_x_pixels;
         pixel_x++)
    {
      float alpha = color.a / 255.0f;

      if (pixel_x == start_x_pixels)
      {
        alpha *= (start_x_pixels + 1) - start_x;
      }
      if (pixel_x == end_x_pixels)
      {
        alpha *= end_x - end_x_pixels;
      }
      if (pixel_y == start_y_pixels)
      {
        alpha *= (start_y_pixels + 1) - start_y;
      }
      if (pixel_y == end_y_pixels)
      {
        alpha *= end_y - end_y_pixels;
      }

      uint32_t pixel_pos = (pixel_y * WINDOW_WIDTH) + pixel_x;

      V4 prev_color = decomposeColor(pixels[pixel_pos]);
      V4 new_color = (alpha * color) + ((1.0f - alpha) * prev_color);

      uint32_t new_color_int = composeColor(new_color);

      pixels[pixel_pos] = new_color_int;
    }
  }
}


void
render_cells(GameMemory * game_memory, uint32_t * pixels, Mouse mouse, Maze * maze)
{
  for (uint32_t cell_y = 0;
       cell_y < maze->height;
       cell_y++)
  {
    for (uint32_t cell_x = 0;
         cell_x < maze->width;
         cell_x++)
    {
      Cell * cell = get_cell(game_memory, maze, cell_x, cell_y);

      V4 color = (V4){};
      switch (cell->type)
      {
        case CELL_NULL:     color = (V4){0xFF, 0x00, 0x00, 0x00};
          break;
        case CELL_START:    color = (V4){0xFF, 0x33, 0xAA, 0x55};
          break;
        case CELL_PATH:     color = (V4){0xFF, 0x55, 0x88, 0x22};
          break;
        case CELL_WALL:     color = (V4){0xFF, 0x33, 0x33, 0x33};
          break;
        case CELL_HOLE:     color = (V4){0xFF, 0xBB, 0x66, 0x44};
          break;
        case CELL_SPLITTER: color = (V4){0xFF, 0x22, 0x44, 0x99};
          break;
        case CELL_FUNCTION: color = (V4){0xFF, 0x66, 0x77, 0x88};
          break;
        case CELL_ONCE:     color = (V4){0xFF, 0x88, 0x77, 0x66};
          break;
        case CELL_SIGNAL:   color = (V4){0xFF, 0x99, 0x99, 0x22};
          break;
        case CELL_INC:      color = (V4){0xFF, 0x33, 0x99, 0x22};
          break;
        case CELL_DEC:      color = (V4){0xFF, 0x99, 0x33, 0x22};
          break;
        case CELL_UP:       color = (V4){0xFF, 0x22, 0x00, 0x00};
          break;
        case CELL_DOWN:     color = (V4){0xFF, 0x00, 0x22, 0x00};
          break;
        case CELL_LEFT:     color = (V4){0xFF, 0x00, 0x00, 0x22};
          break;
        case CELL_RIGHT:    color = (V4){0xFF, 0x00, 0x22, 0x22};
          break;
        case CELL_PAUSE:    color = (V4){0xFF, 0x88, 0x88, 0x11};
          break;
      }

      // if ((mouse.x >= x) &&
      //     (mouse.x < x + width) &&
      //     (mouse.y >= y) &&
      //     (mouse.y < y + height))
      // {
      //   uint32_t color_a = ((color >> 24) & 0xFF);
      //   uint32_t color_r = ((color >> 16) & 0xFF);
      //   uint32_t color_g = ((color >> 8) & 0xFF);
      //   uint32_t color_b = ((color >> 0) & 0xFF);

      //   if (color_r <= (0xFF - 0x20))
      //   {
      //     color_r += 0x20;
      //   }
      //   if (color_g <= (0xFF - 0x20))
      //   {
      //     color_g += 0x20;
      //   }
      //   if (color_b <= (0xFF - 0x20))
      //   {
      //     color_b += 0x20;
      //   }

      //   color = (color_a << 24) | (color_r << 16) | (color_g << 8) | (color_b << 0);
      // }

      // Into world space
      uint32_t x = (cell_x * CELL_SPACING);
      uint32_t y = (cell_y * CELL_SPACING);
      uint32_t width = (CELL_SPACING - CELL_MARGIN);
      uint32_t height = (CELL_SPACING - CELL_MARGIN);

      draw_box(pixels,
               x, y,
               width, height,
               color);
    }
  }
}


void
init_car_mem(Cars * cars)
{
  Car * car = cars->cars;
  for (uint32_t car_index = 0;
       car_index < MAX_CARS;
       ++car_index, ++car)
  {
    car->value = 0;
  }
  cars->next_free = 0;
}


Car *
add_car(Cars * cars, uint32_t x, uint32_t y, Direction direction = UP)
{
  assert(cars->next_free != MAX_CARS);

  Car * car = cars->cars + cars->next_free;
  ++cars->next_free;

  car->update = false;
  car->x = x;
  car->y = y;
  car->direction = direction;

  return car;
}


void
rm_car(Cars * cars, uint32_t car_index)
{
  --cars->next_free;

  Car * new_slot = cars->cars + car_index;
  Car * last_car = cars->cars + cars->next_free;

  *new_slot = *last_car;
}


void
update_cars(GameMemory * game_memory, uint32_t * pixels, uint32_t df, uint32_t frame_count,
            Keys keys, Mouse mouse, Maze * maze, Cars * cars)
{

  Car * car = cars->cars;
  for (uint32_t car_index = 0;
       car_index < cars->next_free;
       ++car_index, ++car)
  {

    if (car->update)
    {

      // TODO: Animation
      // float delta_move_per_s = CELL_SPACING;
      // float delta_move_per_us = delta_move_per_s / seconds_in_u(1);
      // float delta_move_per_frame = delta_move_per_us * df;
      // car->x += delta_move_per_frame;

      uint32_t cell_x = car->x / CELL_SPACING;
      uint32_t cell_y = car->y / CELL_SPACING;

      Cell * current_cell = get_cell(game_memory, maze, cell_x, cell_y);

      // TODO: Deal with race cars (conditions)

      switch (current_cell->type)
      {
        case (CELL_NULL):
        {
          printf("Null\n");
          invalid_code_path;
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
          add_car(cars, car->x, car->y, RIGHT);
          car->direction = LEFT;
        } break;

        case (CELL_FUNCTION):
        {
          printf("Function\n");
        } break;

        case (CELL_ONCE):
        {
          printf("Once\n");
          current_cell->type = CELL_WALL;
        } break;

        case (CELL_SIGNAL):
        {
          printf("Once\n");
        } break;

        case (CELL_INC):
        {
          printf("Increment\n");
          ++car->value;
        } break;

        case (CELL_DEC):
        {
          printf("Decrement\n");
          --car->value;
        } break;

        case (CELL_UP):
        {
          printf("Up\n");
          car->direction = UP;
        } break;

        case (CELL_DOWN):
        {
          printf("Down\n");
          car->direction = DOWN;
        } break;

        case (CELL_LEFT):
        {
          printf("Left\n");
          car->direction = LEFT;
        } break;

        case (CELL_RIGHT):
        {
          printf("Right\n");
          car->direction = RIGHT;
        } break;

        case (CELL_PAUSE):
        {
          printf("Pause\n");
          // TODO: Actually pause...
        } break;
      }
    }
  }

  // TODO: Stop looping through them so many times!

  car = cars->cars;
  for (uint32_t car_index = 0;
       car_index < cars->next_free;
       ++car_index, ++car)
  {
    car->update = true;
  }

  car = cars->cars;
  for (uint32_t car_index = 0;
       car_index < cars->next_free;
       ++car_index, ++car)
  {

    // Car movements
    if (car->direction == STATIONARY)
    {
      // They're stuck this way, FOREVER!
    }
    else
    {
      uint32_t cell_x = car->x / CELL_SPACING;
      uint32_t cell_y = car->y / CELL_SPACING;

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
      walls[UP]    = get_cell(game_memory, maze, cell_x, (cell_y + 1))->type == CELL_WALL;
      walls[DOWN]  = get_cell(game_memory, maze, cell_x, (cell_y - 1))->type == CELL_WALL;
      walls[LEFT]  = get_cell(game_memory, maze, (cell_x - 1), cell_y)->type == CELL_WALL;
      walls[RIGHT] = get_cell(game_memory, maze, (cell_x + 1), cell_y)->type == CELL_WALL;

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
  }
}


void
render_cars(uint32_t * pixels, uint32_t df, Cars * cars)
{
  for (uint32_t car_index = 0;
       car_index < cars->next_free;
       ++car_index)
  {
    Car * car = cars->cars + car_index;

    // Render
    uint32_t x = car->x + ((CELL_SPACING - CELL_MARGIN - CAR_SIZE) / 2);
    uint32_t y = car->y + ((CELL_SPACING - CELL_MARGIN - CAR_SIZE) / 2);

    draw_box(pixels, x, y, CAR_SIZE, CAR_SIZE, (V4){0x00, 0x99, 0x22, 0x77});
  }
}


void
update_and_render(GameMemory * game_memory, uint32_t * pixels, uint32_t df, uint32_t frame_count,
                  Keys keys, Mouse mouse, Maze * maze, Cars * cars)
{
  render_cells(game_memory, pixels, mouse, maze);

  if (frame_count % 2 == 0)
  {
    update_cars(game_memory, pixels, df, frame_count, keys, mouse, maze, cars);
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
  init_mem(&game_memory, TOTAL_MEMORY);

  // The pixel buffer
  uint32_t * pixels = take_struct_mem(&game_memory, uint32_t, (WINDOW_WIDTH * WINDOW_HEIGHT));

  Maze * maze = parse(&game_memory, MAZE_FILENAME);

  printf("Collisions: %d\n", maze->collisions);

  // The car list
  Cars * cars = take_struct_mem(&game_memory, Cars, 1);
  init_car_mem(cars);

  // TODO: When the cells are stored spatially properly, use this for looping through the cells:
  // // Add start cars
  // MazeBlock * block = maze->start;
  // for (uint32_t block_index = 0;
  //      block->next;
  //      ++block)
  // {
  //   Cell * cell = block->cells;
  //   for (uint32_t cell_index = 0;
  //        cell_index < array_count(block->cells);
  //        ++cell_index, ++cell)
  //   {
  //     if (cell->type == CELL_START)
  //     {
  //       Car * car = add_car(cars, cell->x, cell->y);
  //     }
  //   }
  //   block = block->next;
  // }

  for (uint32_t cell_y = 0;
       cell_y < maze->height;
       ++cell_y)
  {
    for (uint32_t cell_x = 0;
         cell_x < maze->width;
         ++cell_x)
    {
      Cell * cell = get_cell(&game_memory, maze, cell_x, cell_y);
      if (cell->type == CELL_START)
      {
        add_car(cars, (CELL_SPACING * cell_x), (CELL_SPACING * cell_y));
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

  // Initialise mouse
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

      update_and_render(&game_memory, pixels, delta_frame, frame_count, keys, mouse, maze, cars);

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
