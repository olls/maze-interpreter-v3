#include <sys/time.h>

#include <time.h>
#include <SDL2/SDL.h>

#include "maze-interpreter.h"

#include "render.cpp"
#include "blocks.cpp"
#include "parser.cpp"


uint64_t
get_us()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((uint64_t)tv.tv_sec * (uint64_t)seconds_in_u(1)) + (uint64_t)tv.tv_usec;
}


Car *
add_car(Cars * cars, uint32_t cell_x, uint32_t cell_y, Direction direction = UP)
{
  assert(cars->next_free != MAX_CARS);

  Car * car = cars->cars + cars->next_free;
  ++cars->next_free;

  car->update = false;
  car->cell_x = cell_x;
  car->cell_y = cell_y;
  car->direction = direction;

  return car;
}


void
rm_car(Cars * cars, uint32_t car_index)
{
  cars[car_index] = cars[--cars->next_free];
}


void
update_cars(uint32_t df, uint32_t frame_count,
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
      // TODO: Deal with race cars (conditions) ??

      Cell * current_cell = get_cell(maze, car->cell_x, car->cell_y);

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
          add_car(cars, car->cell_x, car->cell_y, RIGHT);
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
    else
    {
      // NOTE: Doesn't update cars which have been added this tick.
      car->update = true;
    }
  }

  // TODO: Stop looping through them so many times!

  car = cars->cars;
  for (uint32_t car_index = 0;
       car_index < cars->next_free;
       ++car_index, ++car)
  {
    if (car->direction == STATIONARY)
    {
      // They're stuck this way, FOREVER!
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

      // TODO: Maybe grab the cells in chunks, so we don't have to get the
      //       cells multiple times...

      bool walls[4];
      walls[UP]    = get_cell(maze, car->cell_x, (car->cell_y + 1))->type == CELL_WALL;
      walls[DOWN]  = get_cell(maze, car->cell_x, (car->cell_y - 1))->type == CELL_WALL;
      walls[LEFT]  = get_cell(maze, (car->cell_x - 1), car->cell_y)->type == CELL_WALL;
      walls[RIGHT] = get_cell(maze, (car->cell_x + 1), car->cell_y)->type == CELL_WALL;

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
            ++car->cell_y;
          } break;

          case DOWN:
          {
            --car->cell_y;
          } break;

          case LEFT:
          {
            --car->cell_x;
          } break;

          case RIGHT:
          {
            ++car->cell_x;
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


V2
cell_coord_to_world(GameSetup * setup, uint32_t cell_x, uint32_t cell_y)
{
  // TODO: Is there any point in world space any more???
  //       Cars have cell_x/y and offset, is that all we need? (Help
  //       with precision?)
  V2 result = ((V2){cell_x, cell_y} * setup->cell_spacing);
  return result;
}


void
render_cars(GameSetup * setup, PixelColor * pixels, Rectangle render_region, V2 screen_offset, uint32_t df, Cars * cars)
{
  uint32_t car_raduis = (setup->cell_spacing - (setup->cell_spacing * setup->cell_margin)) * 0.35f;
  // TODO: Loop through only relevant cars?
  //       i.e.: spacial partitioning the storage.
  //       Store the cars in the quad-tree?
  for (uint32_t car_index = 0;
       car_index < cars->next_free;
       ++car_index)
  {
    Car * car = cars->cars + car_index;
    // NOTE: Car centred on coord
    V2 pos = cell_coord_to_world(setup, car->cell_x, car->cell_y) + car->offset + screen_offset;
    draw_circle(setup, pixels, render_region, pos, car_raduis, (V4){1, 0x99, 0x22, 0x77});
  }
}


void
render_cells_in_tree(GameSetup * setup, PixelColor * pixels, Rectangle render_region,
                     V2 screen_offset, Mouse mouse, Maze * maze, QuadTree * tree)
{
  bool selected = false;
  // TODO: IMPORTANT: There ARE bugs in the 'overlaps' pruning of the
  //                  tree...
  // if (tree && overlaps(render_region - 140000, (tree->bounds * setup->cell_spacing)))
  // if (tree && overlaps(render_region - 15000, (tree->bounds * setup->cell_spacing)))
  if (tree && overlaps(render_region, (tree->bounds * setup->cell_spacing) + screen_offset))
  {

    for (uint32_t cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell * cell = tree->cells + cell_index;

      V4 color = (V4){};
      switch (cell->type)
      {
        case CELL_NULL:     color = (V4){1, 0x00, 0x00, 0x00};
          break;
        case CELL_START:    color = (V4){1, 0x33, 0xAA, 0x55};
          break;
        case CELL_PATH:     color = (V4){1, 0x55, 0x88, 0x22};
          break;
        case CELL_WALL:     color = (V4){1, 0x33, 0x33, 0x33};
          break;
        case CELL_HOLE:     color = (V4){1, 0xBB, 0x66, 0x44};
          break;
        case CELL_SPLITTER: color = (V4){1, 0x22, 0x44, 0x99};
          break;
        case CELL_FUNCTION: color = (V4){1, 0x66, 0x77, 0x88};
          break;
        case CELL_ONCE:     color = (V4){1, 0x88, 0x77, 0x66};
          break;
        case CELL_SIGNAL:   color = (V4){1, 0x99, 0x99, 0x22};
          break;
        case CELL_INC:      color = (V4){1, 0x33, 0x99, 0x22};
          break;
        case CELL_DEC:      color = (V4){1, 0x99, 0x33, 0x22};
          break;
        case CELL_UP:       color = (V4){1, 0x22, 0x00, 0x00};
          break;
        case CELL_DOWN:     color = (V4){1, 0x00, 0x22, 0x00};
          break;
        case CELL_LEFT:     color = (V4){1, 0x00, 0x00, 0x22};
          break;
        case CELL_RIGHT:    color = (V4){1, 0x00, 0x22, 0x22};
          break;
        case CELL_PAUSE:    color = (V4){1, 0x88, 0x88, 0x11};
          break;
      }

      uint32_t cell_radius = (setup->cell_spacing - (setup->cell_spacing * setup->cell_margin)) / 2;

      // NOTE: Tile centred on coord
      V2 world_pos = cell_coord_to_world(setup, cell->x, cell->y);
      V2 world_screen_pos = world_pos + screen_offset;
      Rectangle cell_bounds = rectangle(world_screen_pos, cell_radius);

      if (in_rectangle(((V2){mouse.x, mouse.y} * setup->world_per_pixel), cell_bounds))
      {
        selected = true;
        if (color.r <= (0xFF - 0x20))
        {
          color.r += 0x20;
        }
        if (color.g <= (0xFF - 0x20))
        {
          color.g += 0x20;
        }
        if (color.b <= (0xFF - 0x20))
        {
          color.b += 0x20;
        }
      }

      draw_box(setup, pixels, render_region, cell_bounds, color);
    }

#ifdef DEBUG
    V4 box_color = (V4){0.1f, 0, 0, 0};
    Rectangle world_tree_bounds = (tree->bounds * setup->cell_spacing) + screen_offset;
    if (selected)
    {
      box_color.a = 1;
      box_color.r = 0xFF;
    }
    draw_box_outline(setup, pixels, render_region, world_tree_bounds, box_color);
#endif

    render_cells_in_tree(setup, pixels, render_region, screen_offset, mouse, maze, tree->top_right);
    render_cells_in_tree(setup, pixels, render_region, screen_offset, mouse, maze, tree->top_left);
    render_cells_in_tree(setup, pixels, render_region, screen_offset, mouse, maze, tree->bottom_right);
    render_cells_in_tree(setup, pixels, render_region, screen_offset, mouse, maze, tree->bottom_left);
  }
}


void
render_cells(GameSetup * setup, PixelColor * pixels, Rectangle render_region, V2 screen_offset, Mouse mouse, Maze * maze)
{
  render_cells_in_tree(setup, pixels, render_region, screen_offset, mouse, maze, &(maze->tree));
}


void
update_and_render(GameMemory * game_memory, GameSetup * setup, PixelColor * pixels, uint32_t df, uint32_t frame_count,
                  Keys keys, Mouse mouse, Maze * maze, Cars * cars)
{
  uint32_t old_world_per_pixel = setup->world_per_pixel;
  setup->zoom += mouse.scroll.y;
  setup->world_per_pixel = squared(setup->zoom);
  if (mouse.scroll.y > 0 && ((setup->world_per_pixel > MAX_WORLD_PER_PIXEL) ||
                             (setup->world_per_pixel < old_world_per_pixel)))
  {
    setup->zoom -= mouse.scroll.y;
    mouse.scroll.y = 0;
    setup->world_per_pixel = MAX_WORLD_PER_PIXEL;
  }
  else if (mouse.scroll.y < 0 && ((setup->world_per_pixel < MIN_WORLD_PER_PIXEL) ||
                                  (setup->world_per_pixel > old_world_per_pixel)))
  {
    setup->zoom -= mouse.scroll.y;
    mouse.scroll.y = 0;
    setup->world_per_pixel = MIN_WORLD_PER_PIXEL;
  }

  if (frame_count % 2 == 0)
  {
    update_cars(df, frame_count, keys, mouse, maze, cars);
  }

  V2 screen_offset = (V2){0.5, 0.5} * setup->cell_spacing;
  if (mouse.l_down)
  {
    setup->last_mouse = (V2){mouse.x, mouse.y};
  }
  // NOTE: If mouse hasn't been set yet.
  if (setup->last_mouse >= (V2){0, 0})
  {
    screen_offset += (setup->world_per_pixel * ((V2){setup->window_width, setup->window_height} - setup->last_mouse)) - ((V2){maze->width, maze->height} * setup->cell_spacing * 0.5f);
  }

  Rectangle render_region;
  render_region.start = (V2){0, 0};
  render_region.end = (V2){setup->window_width, setup->window_height} * setup->world_per_pixel;

  render_cells(setup, pixels, render_region, screen_offset, mouse, maze);
  render_cars(setup, pixels, render_region, screen_offset, df, cars);
}


int
main(int32_t argc, char * argv[])
{
  srand(time(NULL));

  SDL_Init(SDL_INIT_VIDEO);

  SDL_WindowFlags flags = SDL_WINDOW_SHOWN;
  uint32_t window_width = 0;
  uint32_t window_height = 0;
  if (FULLSCREEN)
  {
    flags = (SDL_WindowFlags)(flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
  }
  else
  {
    window_width = 1024;
    window_height = 600;
  }

  SDL_Window * window = SDL_CreateWindow("A Maze Thingy",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, flags);

  if (FULLSCREEN)
  {
    int32_t display_index = SDL_GetWindowDisplayIndex(window);
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
    window_width = window_rect.w;
    window_height = window_rect.h;
  }

  GameSetup setup_;
  GameSetup * setup = &setup_;
  setup->window_width = window_width;
  setup->window_height = window_height;
  // TODO: Should world coords be floats now we are using uint32s for
  //       the cell position?
  setup->zoom = 16; // NOTE: Square-root of MIN_WORLD_PER_PIXEL: gives max size.
  setup->cell_spacing = 100000;
  setup->cell_margin = 0.2f;
  setup->last_mouse = (V2){-1, -1};

  printV((V2){setup->window_width, setup->window_height});

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture * texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STATIC, setup->window_width, setup->window_height);

  // Init game memory
  GameMemory game_memory;
  init_mem(&game_memory, TOTAL_MEMORY);

  // The pixel buffer
  PixelColor * pixels = take_struct_mem(&game_memory, PixelColor, (setup->window_width * setup->window_height));

  Maze * maze = parse(&game_memory, MAZE_FILENAME);

  // The car list
  Cars * cars = take_struct_mem(&game_memory, Cars, 1);

  // TODO: When the cells are stored spatially properly, use this for
  //       looping through the cells:
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

  // TODO: This is pretty dumb, can we add cars in the parser...?
  //       Noo, add the cars in update_cells, pass something in, or set a
  //       flag in the cell make it only happen once? Or not...
  for (uint32_t cell_y = 0;
       cell_y < maze->height;
       ++cell_y)
  {
    for (uint32_t cell_x = 0;
         cell_x < maze->width;
         ++cell_x)
    {
      Cell * cell = get_cell(maze, cell_x, cell_y);
      if (cell->type == CELL_START)
      {
        add_car(cars, cell_x, cell_y);
      }
    }
  }

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

  Mouse mouse;
  mouse.x = -1;
  mouse.y = -1;
  mouse.l_down = false;
  mouse.r_down = false;
  mouse.scroll = (V2){0, 0};

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
          mouse.y = setup->window_height - event.motion.y;
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
        case SDL_MOUSEWHEEL:
        {
          mouse.scroll.x -= event.wheel.x;
          mouse.scroll.y -= event.wheel.y;
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

    if (now >= last_frame + (seconds_in_u(1)/FPS))
    {
      delta_frame = now - last_frame;
      last_frame = now;
      frame_count++;

      for (uint32_t screen_y = 0;
           screen_y < setup->window_height;
           screen_y++)
      {
        for (uint32_t screen_x = 0;
             screen_x < setup->window_width;
             screen_x++)
        {
          pixels[screen_y * setup->window_width + screen_x] = (PixelColor){255, 255, 255};
        }
      }

      update_and_render(&game_memory, setup, pixels, delta_frame, frame_count, keys, mouse, maze, cars);

      // Flip pixels
      for (uint32_t pixel_y = 0;
           pixel_y < setup->window_height / 2;
           pixel_y++)
      {
        for (uint32_t pixel_x = 0;
             pixel_x < setup->window_width;
             pixel_x++)
        {
          uint32_t top_pixel_pos = pixel_y * setup->window_width + pixel_x;
          PixelColor top_pixel = pixels[top_pixel_pos];

          uint32_t bottom_pixel_pos = (setup->window_height - pixel_y - 1) * setup->window_width + pixel_x;

          pixels[top_pixel_pos] = pixels[bottom_pixel_pos];
          pixels[bottom_pixel_pos] = top_pixel;
        }
      }

      SDL_UpdateTexture(texture, NULL, pixels, setup->window_width * sizeof(PixelColor));

      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
    }

    mouse.scroll -= mouse.scroll / 4;

  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  free(game_memory.memory);

  return 0;
}
