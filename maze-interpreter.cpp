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
render_cars(GameState *game_state, FrameBuffer *frame_buffer, RenderBasis *render_basis, Cars *cars)
{
  u32 car_raduis = (game_state->cell_spacing - (game_state->cell_spacing * game_state->cell_margin)) * 0.35f;
  // TODO: Loop through only relevant cars?
  //       i.e.: spacial partitioning the storage.
  //       Store the cars in the quad-tree?
  for (u32 car_index = 0;
       car_index < cars->next_free;
       ++car_index)
  {
    Car *car = cars->cars + car_index;
    V2 pos = cell_coord_to_world(game_state, car->cell_x, car->cell_y) + car->offset;
    draw_circle(frame_buffer, render_basis, pos, car_raduis, (V4){1, 0x99, 0x22, 0x77});
  }
}


void
update_cell(Cell *cell, Cars *cars, b32 first_frame=false)
{
  if (first_frame)
  {
    if (cell->type == CELL_START)
    {
      add_car(cars, cell->x, cell->y);
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

    u32 cell_radius = (game_state->cell_spacing - (game_state->cell_spacing * game_state->cell_margin)) / 2;

    // NOTE: Tile centred on coord
    V2 world_pos = cell_coord_to_world(game_state, cell->x, cell->y);
    Rectangle cell_bounds = rectangle(world_pos, cell_radius);

    if (in_rectangle(((V2){mouse->x, mouse->y} * game_state->world_per_pixel), cell_bounds))
    {
      selected = true;
      color = clamp(add_color(color, 0x20), 0xff);
    }

    draw_box(frame_buffer, render_basis, cell_bounds, color);
  }

  return selected;
}


void update_and_render_cells(GameState *game_state, Mouse *mouse, FrameBuffer *frame_buffer, RenderBasis *render_basis, QuadTree * tree)
{
  b32 selected = false;
  // TODO: IMPORTANT: There ARE bugs in the 'overlaps' pruning of the
  //                  tree...
  if (tree && overlaps(render_basis->clip_region, (tree->bounds * game_state->cell_spacing) + render_basis->origin))
  {

    for (u32 cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell *cell = tree->cells + cell_index;

      update_cell(cell, &(game_state->cars), (game_state->frame_count == 0));
      selected = render_cell(cell, game_state, mouse, frame_buffer, render_basis);
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

    update_and_render_cells(game_state, mouse, frame_buffer, render_basis, tree->top_right);
    update_and_render_cells(game_state, mouse, frame_buffer, render_basis, tree->top_left);
    update_and_render_cells(game_state, mouse, frame_buffer, render_basis, tree->bottom_right);
    update_and_render_cells(game_state, mouse, frame_buffer, render_basis, tree->bottom_left);
  }
}


void
render(GameState *game_state, FrameBuffer *frame_buffer, Mouse *mouse)
{
  // TODO: Center zoom on mouse

  u32 old_world_per_pixel = game_state->world_per_pixel;

  game_state->zoom += mouse->scroll.y;
  game_state->world_per_pixel = squared(game_state->zoom);

  if (mouse->scroll.y > 0 && ((game_state->world_per_pixel >= MAX_WORLD_PER_PIXEL) ||
                             (game_state->world_per_pixel < old_world_per_pixel)))
  {
    game_state->zoom -= mouse->scroll.y;
    game_state->world_per_pixel = MAX_WORLD_PER_PIXEL;
  }
  else if (mouse->scroll.y < 0 && ((game_state->world_per_pixel <= MIN_WORLD_PER_PIXEL) ||
                                  (game_state->world_per_pixel > old_world_per_pixel)))
  {
    game_state->zoom -= mouse->scroll.y;
    game_state->world_per_pixel = MIN_WORLD_PER_PIXEL;
  }

  V2 d_mouse = (V2){mouse->x, mouse->y} - game_state->last_mouse_pos;
  game_state->last_mouse_pos = (V2){mouse->x, mouse->y};

  if (mouse->l_down)
  {
    game_state->maze_pos += d_mouse;
  }

  RenderBasis render_basis;
  render_basis.scale = game_state->world_per_pixel;

  render_basis.origin = game_state->world_per_pixel * game_state->maze_pos;

  render_basis.clip_region.start = (V2){0, 0};
  render_basis.clip_region.end = (V2){frame_buffer->width, frame_buffer->height} * game_state->world_per_pixel;

  update_and_render_cells(game_state, mouse, frame_buffer, &render_basis, &(game_state->maze.tree));
  render_cars(game_state, frame_buffer, &render_basis, &(game_state->cars));
}


void
init_game(Memory *memory, GameState *game_state, u32 argc, char *argv[])
{
  game_state->init = true;

  // TODO: Should world coords be r32s now we are using uint32s for
  //       the cell position?
  // NOTE: Somewhere between the sqrt( [ MIN, MAX ]_WORLD_PER_PIXEL )
  game_state->zoom = 64;
  game_state->cell_spacing = 100000;
  game_state->cell_margin = 0.2f;

  if (argc > 1)
  {
    parse(&(game_state->maze), memory, argv[1]);
  }
  else
  {
    parse(&(game_state->maze), memory, MAZE_FILENAME);
  }

  game_state->cars.next_free = 0;
}


void
update_and_render(Memory *memory, GameState *game_state, FrameBuffer *frame_buffer, Keys *keys, Mouse *mouse, u64 time_us, u32 argc, char *argv[])
{
  if (!game_state->init)
  {
    init_game(memory, game_state, argc, argv);
  }

  update_cars(game_state, time_us);

  Rectangle render_region_pixels;
  render_region_pixels.start = (V2){0, 0};
  render_region_pixels.end = (V2){frame_buffer->width, frame_buffer->height};

  RenderBasis clear_basis;
  clear_basis.origin = (V2){0 ,0};
  clear_basis.scale = 1;
  clear_basis.clip_region = render_region_pixels;

  fast_draw_box(frame_buffer, &clear_basis, render_region_pixels, (V4){1, 0xff, 0xff, 0xff});

  render(game_state, frame_buffer, mouse);

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