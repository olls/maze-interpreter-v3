#include "render.cpp"
#include "functions.cpp"
#include "maze.cpp"
#include "parser.cpp"
#include "cars.cpp"


V2
cell_coord_to_world(GameState *game_state, u32 cell_x, u32 cell_y)
{
  // TODO: Is there any point in world space any more???
  //       Cars have cell_x/y and offset, is that all we need? (Help
  //       with precision?)
  V2 result = ((V2){cell_x, cell_y} * game_state->cell_spacing);
  return result;
}


void
draw_car(GameState *game_state, FrameBuffer *frame_buffer, RenderBasis *render_basis, V2 pos, u64 time_us)
{
  u32 car_raduis = (game_state->cell_spacing - (game_state->cell_spacing * game_state->cell_margin)) * 0.35f;

  draw_circle(frame_buffer, render_basis, pos, car_raduis + (sin(time_us / 100000) * car_raduis * 0.5), (V4){1, 0.60, 0.13, 0.47});
}


void
render_cars(GameState *game_state, FrameBuffer *frame_buffer, RenderBasis *render_basis, Cars *cars, u64 time_us)
{
  // TODO: Loop through only relevant cars?
  //       i.e.: spacial partitioning the storage.
  //       Store the cars in the quad-tree?
  CarsBlock *cars_block = cars->first_block;
  while (cars_block != 0)
  {
    for (u32 car_index = 0;
         car_index < cars_block->next_free_in_block;
         ++car_index)
    {
      Car *car = cars_block->cars + car_index;

      V2 pos = cell_coord_to_world(game_state, car->cell_x, car->cell_y) + car->offset;
      draw_car(game_state, frame_buffer, render_basis, pos, time_us);

#if 0 // Show real location
      pos = cell_coord_to_world(game_state, car->target_cell_x, car->target_cell_y);
      draw_circle(frame_buffer, render_basis, pos, car_raduis, (V4){1, 0, 1, 0});
#endif
    }
    cars_block = cars_block->next_block;
  }
}


void
update_cell(Memory *memory, Cell *cell, Cars *cars, b32 first_frame=false)
{
  if (first_frame)
  {
    if (cell->type == CELL_START)
    {
      add_car(memory, cars, cell->x, cell->y);
    }
  }
}


b32
render_cell(Cell *cell, GameState *game_state, Mouse *mouse, FrameBuffer *frame_buffer, RenderBasis *render_basis)
{
  b32 selected = false;
  if (cell->type != CELL_WALL)
  {

    V4 color = (V4){};
    switch (cell->type)
    {
      case CELL_NULL:     color = (V4){1, 0.00, 0.00, 0.00};
        break;
      case CELL_START:    color = (V4){1, 0.20, 0.67, 0.33};
        break;
      case CELL_PATH:     color = (V4){1, 0.33, 0.53, 0.13};
        break;
      case CELL_WALL:     color = (V4){1, 0.20, 0.20, 0.20};
        break;
      case CELL_HOLE:     color = (V4){1, 0.73, 0.40, 0.27};
        break;
      case CELL_SPLITTER: color = (V4){1, 0.13, 0.27, 0.60};
        break;
      case CELL_FUNCTION: color = (V4){1, 0.40, 0.47, 0.53};
        break;
      case CELL_ONCE:     color = (V4){1, 0.53, 0.47, 0.40};
        break;
      case CELL_SIGNAL:   color = (V4){1, 0.60, 0.60, 0.13};
        break;
      case CELL_INC:      color = (V4){1, 0.20, 0.60, 0.13};
        break;
      case CELL_DEC:      color = (V4){1, 0.60, 0.20, 0.13};
        break;
      case CELL_UP:       color = (V4){1, 0.13, 0.00, 0.00};
        break;
      case CELL_DOWN:     color = (V4){1, 0.00, 0.13, 0.00};
        break;
      case CELL_LEFT:     color = (V4){1, 0.00, 0.00, 0.13};
        break;
      case CELL_RIGHT:    color = (V4){1, 0.00, 0.13, 0.13};
        break;
      case CELL_PAUSE:    color = (V4){1, 0.53, 0.53, 0.07};
        break;
    }

    u32 cell_radius = (game_state->cell_spacing - (game_state->cell_spacing * game_state->cell_margin)) / 2;

    // NOTE: Tile centred on coord
    V2 world_pos = cell_coord_to_world(game_state, cell->x, cell->y);
    Rectangle cell_bounds = rectangle(world_pos, cell_radius);

    // TODO: Unproject the mouse coords correctly
    if (in_rectangle(((V2){mouse->x, mouse->y} * game_state->world_per_pixel), cell_bounds))
    {
      selected = true;
      color = clamp(add_color(color, 0x20), 0xff);
    }

    draw_box(frame_buffer, render_basis, cell_bounds, color);
  }

  return selected;
}


void update_and_render_cells(Memory *memory, GameState *game_state, Mouse *mouse, FrameBuffer *frame_buffer, RenderBasis *render_basis, QuadTree * tree)
{
  b32 selected = false;
  // TODO: IMPORTANT: There ARE bugs in the 'overlaps' pruning of the
  //                  tree...
  if (tree)
  {
    b32 on_screen = overlaps(render_basis->clip_region, (tree->bounds * game_state->cell_spacing) + render_basis->origin);

    for (u32 cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell *cell = tree->cells + cell_index;

      update_cell(memory, cell, &(game_state->cars), (game_state->frame_count == 0));

      if (on_screen)
      {
        selected = render_cell(cell, game_state, mouse, frame_buffer, render_basis);
      }
    }

#if 0
    V4 box_color = (V4){0.1f, 0, 0, 0};
    Rectangle world_tree_bounds = (tree->bounds * game_state->cell_spacing) + screen_offset;
    if (selected)
    {
      box_color.a = 1;
      box_color.r = 0xFF;
    }
    draw_box_outline(frame_buffer, game_state->world_per_pixel, render_region, world_tree_bounds, box_color);
#endif

    update_and_render_cells(memory, game_state, mouse, frame_buffer, render_basis, tree->top_right);
    update_and_render_cells(memory, game_state, mouse, frame_buffer, render_basis, tree->top_left);
    update_and_render_cells(memory, game_state, mouse, frame_buffer, render_basis, tree->bottom_right);
    update_and_render_cells(memory, game_state, mouse, frame_buffer, render_basis, tree->bottom_left);
  }
}


void
render(Memory *memory, GameState *game_state, FrameBuffer *frame_buffer, Rectangle render_region_pixels, Mouse *mouse, u64 time_us)
{
  // TODO: Give zoom velocity, to make it smooth

  u32 old_zoom = game_state->zoom;
  game_state->d_zoom += mouse->scroll.y;
  game_state->zoom += game_state->d_zoom;
  game_state->d_zoom *= 0.5f;

  if (mouse->scroll.y > 0 && ((game_state->zoom >= MAX_ZOOM) ||
                              (game_state->zoom < old_zoom)))
  {
    game_state->d_zoom = 0;
    game_state->zoom = MAX_ZOOM;
  }
  else if (mouse->scroll.y < 0 && ((game_state->zoom <= MIN_ZOOM) ||
                                   (game_state->zoom > old_zoom)))
  {
    game_state->d_zoom = 0;
    game_state->zoom = MIN_ZOOM;
  }
  game_state->scale = squared(game_state->zoom / 30.0f);

  V2 screen_mouse_pixels = (V2){mouse->x, mouse->y};
  V2 d_screen_mouse_pixels = screen_mouse_pixels - game_state->last_mouse_pos;

  game_state->last_mouse_pos = screen_mouse_pixels;

  if (mouse->l_down)
  {
    game_state->maze_pos += d_screen_mouse_pixels;
  }

  RenderBasis render_basis = {};
  render_basis.world_per_pixel = game_state->world_per_pixel;

  render_basis.scale_focus = game_state->scale_focus;
  if (game_state->d_zoom > .2f)
  {
    game_state->d_zoom = 0;
    game_state->scale_focus = screen_mouse_pixels;
    render_basis.scale_focus = screen_mouse_pixels;
  }
  render_basis.scale = game_state->scale;

  render_basis.origin = (game_state->maze_pos / render_basis.scale) * game_state->world_per_pixel;

  render_basis.clip_region.start = (V2){0, 0};
  render_basis.clip_region.end = (V2){frame_buffer->width, frame_buffer->height} * game_state->world_per_pixel;

  update_and_render_cells(memory, game_state, mouse, frame_buffer, &render_basis, &(game_state->maze.tree));
  render_cars(game_state, frame_buffer, &render_basis, &(game_state->cars), time_us);
}


void
process_input(Keys *keys, Input *input, u64 time_us)
{
#define SET_INPUT(inp, letter) inp = keys->alpha[letter - 'a']

  SET_INPUT(input->step, 'j');

  if (input->step_mode_switch.value)
  {
    input->step_mode_switch.value = false;
  }
  else if (keys->alpha['s' - 'a'])
  {
    if (time_us >= input->step_mode_switch.last_update + (seconds_in_u(1) / TOGGLE_DEBOUNCE_RATE))
    {
      SET_INPUT(input->step_mode_switch.value, 's');
      input->step_mode_switch.last_update = time_us;
    }
  }

}


void
init_game(Memory *memory, GameState *game_state, u32 argc, char *argv[])
{
  game_state->init = true;

  // TODO: Should world coords be r32s now we are using uint32s for
  //       the cell position?
  // NOTE: Somewhere between the sqrt( [ MIN, MAX ]_WORLD_PER_PIXEL )
  game_state->zoom = 30;
  game_state->world_per_pixel = 64*64;
  game_state->cell_spacing = 100000;
  game_state->cell_margin = 0.2f;
  game_state->single_step = true;

  if (argc > 1)
  {
    parse(&(game_state->maze), memory, argv[1]);
  }
  else
  {
    parse(&(game_state->maze), memory, MAZE_FILENAME);
  }

  game_state->cars.first_block = 0;
  game_state->cars.free_chain = 0;
}


void
update_and_render(Memory *memory, GameState *game_state, FrameBuffer *frame_buffer, Keys *keys, Mouse *mouse, u64 time_us, u32 argc, char *argv[])
{
  if (!game_state->init)
  {
    init_game(memory, game_state, argc, argv);
  }

  process_input(keys, &game_state->input, time_us);

  if (game_state->input.step_mode_switch.value)
  {
    printf("Changing stepping mode\n");
    game_state->single_step = !game_state->single_step;
  }

  update_cars(memory, game_state, &game_state->input, time_us);

  Rectangle render_region_pixels;
  render_region_pixels.start = (V2){0, 0};
  render_region_pixels.end = (V2){frame_buffer->width, frame_buffer->height};

  RenderBasis clear_basis = {};
  clear_basis.origin = (V2){0 ,0};
  clear_basis.world_per_pixel = 1;
  clear_basis.scale = 1;
  clear_basis.clip_region = render_region_pixels;

  fast_draw_box(frame_buffer, &clear_basis, render_region_pixels, (PixelColor){255, 255, 255});

  render(memory, game_state, frame_buffer, render_region_pixels, mouse, time_us);

  mouse->scroll -= mouse->scroll / 6.0f;
  r32 epsilon = 3.0f;
  if (abs(mouse->scroll.y) < epsilon)
  {
    mouse->scroll.y = 0;
  }
  if (abs(mouse->scroll.x) < epsilon)
  {
    mouse->scroll.x = 0;
  }

  ++game_state->frame_count;
}