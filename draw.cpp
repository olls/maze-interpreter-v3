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
draw_car(GameState *game_state, FrameBuffer *frame_buffer, RenderBasis *render_basis, Car *car, u64 time_us, V4 colour = (V4){1, 0.60, 0.13, 0.47})
{
#if 1
  u32 car_raduis = calc_car_radius(game_state);
  V2 pos = cell_coord_to_world(game_state, car->target_cell_x, car->target_cell_y) + car->offset;

  draw_circle(frame_buffer, render_basis, pos, car_raduis, colour);

#else  // NOTE: Displays the id of the car

  u32 cell_size = game_state->cell_spacing - (game_state->cell_spacing*game_state->cell_margin);
  V2 pos = cell_coord_to_world(game_state, car->target_cell_x, car->target_cell_y) + car->offset - (cell_size * .5f);

  u32 n = 6;
  u32 car_diameter = cell_size / n;
  pos += car_diameter * .5f;

  for (u32 i = 0; i <= car->id; ++i)
  {
    draw_circle(frame_buffer, render_basis, pos + (V2){((i%n) * car_diameter), ((i/n) * car_diameter)}, car_diameter*.5f, colour);
  }
#endif
}


void
render_cars(GameState *game_state, FrameBuffer *frame_buffer, RenderBasis *render_basis, Cars *cars, u64 time_us)
{
  // TODO: Loop through only relevant cars?
  //       i.e.: spacial partitioning the storage.
  //       Store the cars in the quad-tree?
  CarsIterator iter = {};
  Car *car;
  while ((car = cars_iterator(cars, &iter)))
  {
#ifdef DEBUG_BLOCK_COLORS
    draw_car(game_state, frame_buffer, render_basis, car, time_us, iter.cars_block->c);
#else
    draw_car(game_state, frame_buffer, render_basis, car, time_us);
#endif
  }
}


b32
render_cell(Cell *cell, GameState *game_state, Mouse *mouse, FrameBuffer *frame_buffer, RenderBasis *render_basis)
{
  b32 selected = false;
  if (cell->type != CELL_WALL)
  {

    u32 color = 0;
    switch (cell->type)
    {
      case CELL_NULL:     color = 000;
        break;
      case CELL_START:    color = 733;
        break;
      case CELL_PATH:     color = 313;
        break;
      case CELL_WALL:     color = 020;
        break;
      case CELL_HOLE:     color = 027;
        break;
      case CELL_SPLITTER: color = 760;
        break;
      case CELL_FUNCTION: color = 753;
        break;
      case CELL_ONCE:     color = 740;
        break;
      case CELL_UP_UNLESS_DETECT:     color = 013;
        break;
      case CELL_DOWN_UNLESS_DETECT:   color = 013;
        break;
      case CELL_LEFT_UNLESS_DETECT:   color = 013;
        break;
      case CELL_RIGHT_UNLESS_DETECT:  color = 013;
        break;
      case CELL_INC:      color = 013;
        break;
      case CELL_DEC:      color = 013;
        break;
      case CELL_UP:       color = 000;
        break;
      case CELL_DOWN:     color = 300;
        break;
      case CELL_LEFT:     color = 013;
        break;
      case CELL_RIGHT:    color = 313;
        break;
      case CELL_PAUSE:    color = 307;
        break;
    }

    u32 cell_radius = (game_state->cell_spacing - (game_state->cell_spacing * game_state->cell_margin)) / 2;

    // NOTE: Tile centred on coord
    V2 world_pos = cell_coord_to_world(game_state, cell->x, cell->y);
    Rectangle cell_bounds = radius_rectangle(world_pos, cell_radius);

    V4 color_multiplier = {1, 1, 1, 1};
    if (in_rectangle((V2){mouse->x, mouse->y}, transform_coord_rect(render_basis, cell_bounds)))
    {
      selected = true;
      color_multiplier.r += 0.5;
      color_multiplier.g += 0.5;
      color_multiplier.b += 0.5;
    }

    V2 scale = (render_basis->scale * cell_radius * 2 / game_state->world_per_pixel) / (V2){game_state->tile.file->width - 1, game_state->tile.file->height - 1};

    blit_bitmap(frame_buffer, &game_state->tile, transform_coord(render_basis, cell_bounds.start), scale, color_multiplier, color);
  }

  return selected;
}


void
render_cells(Memory *memory, GameState *game_state, Mouse *mouse, FrameBuffer *frame_buffer, RenderBasis *render_basis, QuadTree *tree)
{
  b32 selected = false;
  b32 on_screen = false;

  Rectangle cell_clip_region_pixels = (Rectangle){render_basis->clip_region.start, render_basis->clip_region.end} / render_basis->world_per_pixel;

  if (tree)
  {
    on_screen = overlaps(cell_clip_region_pixels, transform_coord_rect(render_basis, grow(tree->bounds, .5) * game_state->cell_spacing));

    for (u32 cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell *cell = tree->cells + cell_index;

      if (on_screen)
      {
        selected |= render_cell(cell, game_state, mouse, frame_buffer, render_basis);
      }
    }

#if 0
    V4 box_color = (V4){0.5f, 0, 0, 0};
    Rectangle world_tree_bounds = (tree->bounds * game_state->cell_spacing);
    if (selected)
    {
      box_color.a = 1;
      box_color.r = 1;
    }
    draw_box_outline(frame_buffer, render_basis, world_tree_bounds, box_color);
#endif

    render_cells(memory, game_state, mouse, frame_buffer, render_basis, tree->top_right);
    render_cells(memory, game_state, mouse, frame_buffer, render_basis, tree->top_left);
    render_cells(memory, game_state, mouse, frame_buffer, render_basis, tree->bottom_right);
    render_cells(memory, game_state, mouse, frame_buffer, render_basis, tree->bottom_left);
  }
}

