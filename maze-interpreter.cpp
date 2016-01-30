#include <sys/time.h>

#include <time.h>
#include <SDL2/SDL.h>

#include "maze-interpreter.h"

#include "render.cpp"
#include "functions.cpp"
#include "maze.cpp"
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
  car->value = 0;
  car->cell_x = cell_x;
  car->cell_y = cell_y;
  car->direction = direction;
  car->pause_left = 0;
  car->unpause_direction = STATIONARY;

  return car;
}


void
rm_car(Cars * cars, uint32_t car_index)
{
  --cars->next_free;
  if (car_index != cars->next_free)
  {
    cars->cars[car_index] = cars->cars[cars->next_free];
  }
}


void
update_cars(uint32_t df, uint32_t frame_count, Keys keys, Maze * maze, Cars * cars)
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
          Function * function = maze->functions + current_cell->function_index;
          printf("Function: %s, %d\n", function->name, (uint32_t)function->type);
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
          if (car->pause_left != 0)
          {
            --car->pause_left;
          }

          if (car->pause_left == 0)
          {
            if (car->unpause_direction == STATIONARY)
            {
              // Start Pause
              car->pause_left = current_cell->pause;
              car->unpause_direction = car->direction;
              car->direction = STATIONARY;
            }
            else
            {
              // End Pause
              car->direction = car->unpause_direction;
              car->unpause_direction = STATIONARY;
            }
          }
          printf("Pause: %d/%d\n", car->pause_left, current_cell->pause);
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
  //         I need to deal with all the movements, before I update
  //         the cars... Therefore two loops are probably
  //         necessary...

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

      V2 dir_components[] = {{ 0, -1}, {0, 1},
                             {-1,  0}, {1, 0}};

      bool can_move = false;
      for (uint32_t direction_index = 0;
           direction_index < 4;
           direction_index++)
      {
        Direction test_direction = directions[direction_index];
        V2 test_comp = dir_components[test_direction];

        Cell * test_cell = get_cell(maze, (car->cell_x + test_comp.x), (car->cell_y + test_comp.y));
        if (test_cell)
        {
          if (test_cell->type != CELL_WALL)
          {
            can_move = true;
            car->direction = test_direction;
            break;
          }
        }
      }

      if (can_move)
      {
        switch (car->direction)
        {
          case UP:
          {
            --car->cell_y;
          } break;

          case DOWN:
          {
            ++car->cell_y;
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
cell_coord_to_world(Game * game, uint32_t cell_x, uint32_t cell_y)
{
  // TODO: Is there any point in world space any more???
  //       Cars have cell_x/y and offset, is that all we need? (Help
  //       with precision?)
  V2 result = ((V2){cell_x, cell_y} * game->cell_spacing);
  return result;
}


void
render_cars(Game * game, PixelColor * pixels, Rectangle render_region, V2 screen_offset, uint32_t df, Cars * cars)
{
  uint32_t car_raduis = (game->cell_spacing - (game->cell_spacing * game->cell_margin)) * 0.35f;
  // TODO: Loop through only relevant cars?
  //       i.e.: spacial partitioning the storage.
  //       Store the cars in the quad-tree?
  for (uint32_t car_index = 0;
       car_index < cars->next_free;
       ++car_index)
  {
    Car * car = cars->cars + car_index;
    V2 pos = cell_coord_to_world(game, car->cell_x, car->cell_y) + car->offset + screen_offset;
    draw_circle(game, pixels, render_region, pos, car_raduis, (V4){1, 0x99, 0x22, 0x77});
  }
}


void
render_cells(Game * game, PixelColor * pixels, Rectangle render_region, V2 screen_offset, QuadTree * tree)
{
  bool selected = false;
  // TODO: IMPORTANT: There ARE bugs in the 'overlaps' pruning of the
  //                  tree...
  // if (tree && overlaps(render_region - 140000, (tree->bounds * game->cell_spacing)))
  // if (tree && overlaps(render_region - 15000, (tree->bounds * game->cell_spacing)))
  if (tree && overlaps(render_region, (tree->bounds * game->cell_spacing) + screen_offset))
  {

    for (uint32_t cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell * cell = tree->cells + cell_index;

      if (cell->type != CELL_WALL)
      {

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

        uint32_t cell_radius = (game->cell_spacing - (game->cell_spacing * game->cell_margin)) / 2;

        // NOTE: Tile centred on coord
        V2 world_pos = cell_coord_to_world(game, cell->x, cell->y);
        V2 world_screen_pos = world_pos + screen_offset;
        Rectangle cell_bounds = rectangle(world_screen_pos, cell_radius);

        if (in_rectangle(((V2){game->mouse.x, game->mouse.y} * game->world_per_pixel), cell_bounds))
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

        draw_box(game, pixels, render_region, cell_bounds, color);
      }
    }

#if 0
    V4 box_color = (V4){0.1f, 0, 0, 0};
    Rectangle world_tree_bounds = (tree->bounds * game->cell_spacing) + screen_offset;
    if (selected)
    {
      box_color.a = 1;
      box_color.r = 0xFF;
    }
    draw_box_outline(game, pixels, render_region, world_tree_bounds, box_color);
#endif

    render_cells(game, pixels, render_region, screen_offset, tree->top_right);
    render_cells(game, pixels, render_region, screen_offset, tree->top_left);
    render_cells(game, pixels, render_region, screen_offset, tree->bottom_right);
    render_cells(game, pixels, render_region, screen_offset, tree->bottom_left);
  }
}


void
add_start_cars(QuadTree * tree, Cars * cars)
{
  // IMPORTANT: Too slow!
  // TODO: - Can we add cars in the parser...?
  //       - Noooo, add the cars in update_cells, pass something in,
  //         or set a flag in the cell make it only happen once? Or
  //         not...
  //       - It will probably be used for more than just start cars?
  //       - This recursive structure is so simple maybe it doesn't
  //         matter that it is duplicated?
  if (tree)
  {
    for (uint32_t cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell * cell = tree->cells + cell_index;

      if (cell->type == CELL_START)
      {
        add_car(cars, cell->x, cell->y);
      }
    }

    add_start_cars(tree->top_right, cars);
    add_start_cars(tree->top_left, cars);
    add_start_cars(tree->bottom_right, cars);
    add_start_cars(tree->bottom_left, cars);
  }
}


void
update_and_render(GameMemory * game_memory,Game * game, PixelColor * pixels, uint32_t df, uint32_t frame_count,
                  Keys keys, Maze * maze, Cars * cars)
{
  if (frame_count % 2 == 0)
  {
    update_cars(df, frame_count, keys, maze, cars);
  }

  uint32_t old_world_per_pixel = game->world_per_pixel;
  game->zoom += game->mouse.scroll.y;
  game->world_per_pixel = squared(game->zoom);
  if (game->mouse.scroll.y > 0 && ((game->world_per_pixel >= MAX_WORLD_PER_PIXEL) ||
                                   (game->world_per_pixel < old_world_per_pixel)))
  {
    game->zoom -= game->mouse.scroll.y;
    game->world_per_pixel = MAX_WORLD_PER_PIXEL;
  }
  else if (game->mouse.scroll.y < 0 && ((game->world_per_pixel <= MIN_WORLD_PER_PIXEL) ||
                                        (game->world_per_pixel > old_world_per_pixel)))
  {
    game->zoom -= game->mouse.scroll.y;
    game->world_per_pixel = MIN_WORLD_PER_PIXEL;
  }

  V2 screen_offset = (V2){0.5, 0.5} * game->cell_spacing;
  if (game->mouse.l_down)
  {
    game->last_mouse_pos = (V2){game->mouse.x, game->mouse.y};
  }
  // NOTE: If mouse hasn't been set yet.
  if (game->last_mouse_pos >= (V2){0, 0})
  {
    screen_offset += (game->world_per_pixel * ((V2){game->window_width, game->window_height} - game->last_mouse_pos)) - ((V2){maze->width, maze->height} * game->cell_spacing * 0.5f);
  }

  Rectangle render_region;
  render_region.start = (V2){0, 0};
  render_region.end = (V2){game->window_width, game->window_height} * game->world_per_pixel;

  render_cells(game, pixels, render_region, screen_offset, &(maze->tree));
  render_cars(game, pixels, render_region, screen_offset, df, cars);
}


int
main(int32_t argc, char * argv[])
{
  srand(time(NULL));

  Game game_ = {};
  Game * game = &game_;

  // TODO: Should world coords be floats now we are using uint32s for
  //       the cell position?
  // NOTE: Somewhere between the sqrt( [ MIN, MAX ]_WORLD_PER_PIXEL )
  game->zoom = 64;
  game->cell_spacing = 100000;
  game->cell_margin = 0.2f;
  game->last_mouse_pos = (V2){-1, -1};

  game->mouse.x = -1;
  game->mouse.y = -1;
  game->mouse.l_down = false;
  game->mouse.r_down = false;
  game->mouse.scroll = (V2){0, 0};

  GameMemory game_memory;
  init_mem(&game_memory, TOTAL_MEMORY);

  Maze * maze;
  if (argc > 1)
  {
    maze = parse(&game_memory, argv[1]);
  }
  else
  {
    maze = parse(&game_memory, MAZE_FILENAME);
  }

  printf("Parsing Done\n");

  // The car list
  Cars * cars = take_struct_mem(&game_memory, Cars, 1);
  cars->next_free = 0;
  add_start_cars(&(maze->tree), cars);

  SDL_Init(SDL_INIT_VIDEO);

  SDL_WindowFlags flags = (SDL_WindowFlags)(SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
  if (FULLSCREEN)
  {
    flags = (SDL_WindowFlags)(flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
  }
  else
  {
    game->window_width = 1024;
    game->window_height = 600;
  }

  SDL_Window * window = SDL_CreateWindow("A Maze Thingy",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, game->window_width, game->window_height, flags);

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
    game->window_width = window_rect.w;
    game->window_height = window_rect.h;
  }

  PixelColor * pixels = take_struct_mem(&game_memory, PixelColor, (game->window_width * game->window_height));

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture * texture = SDL_CreateTexture(renderer,
    SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STATIC, game->window_width, game->window_height);

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
          game->mouse.x = event.motion.x;
          game->mouse.y = event.motion.y;
        } break;

        case SDL_MOUSEBUTTONDOWN:
        {
          switch (event.button.button)
          {
            case SDL_BUTTON_LEFT:
              game->mouse.l_down = true;
              break;
            case SDL_BUTTON_RIGHT:
              game->mouse.r_down = true;
              break;
          }
        } break;

        case SDL_MOUSEBUTTONUP:
        {
          switch (event.button.button)
          {
            case SDL_BUTTON_LEFT:
              game->mouse.l_down = false;
              break;
            case SDL_BUTTON_RIGHT:
              game->mouse.r_down = false;
              break;
          }
        } break;
        case SDL_MOUSEWHEEL:
        {
          game->mouse.scroll.x -= event.wheel.x;
          game->mouse.scroll.y -= event.wheel.y;
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
           screen_y < game->window_height;
           screen_y++)
      {
        for (uint32_t screen_x = 0;
             screen_x < game->window_width;
             screen_x++)
        {
          pixels[screen_y * game->window_width + screen_x] = (PixelColor){255, 255, 255};
        }
      }

      update_and_render(&game_memory, game, pixels, delta_frame, frame_count, keys, maze, cars);

      SDL_UpdateTexture(texture, NULL, pixels, game->window_width * sizeof(PixelColor));
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
    }

    game->mouse.scroll -= game->mouse.scroll / 6.0f;
    float epsilon = 3.0f;
    if (abs(game->mouse.scroll.y) < epsilon)
    {
      game->mouse.scroll.y = 0;
    }
    if (abs(game->mouse.scroll.x) < epsilon)
    {
      game->mouse.scroll.x = 0;
    }
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  free(game_memory.memory);

  return 0;
}
