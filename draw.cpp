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
  u32 car_raduis = calc_car_radius(game_state);
  V2 pos = cell_coord_to_world(game_state, car->target_cell_x, car->target_cell_y) + car->offset;

  draw_circle(frame_buffer, render_basis, pos, car_raduis, colour);

  char str[256];
  fmted_str(str, "%d", car->value);
  draw_string(frame_buffer, render_basis, &game_state->font, pos - 0.5*CHAR_SIZE*game_state->world_per_pixel*0.15, str, 0.15);
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
render_cell(Cell *cell, GameState *game_state, Mouse *mouse, FrameBuffer *frame_buffer, RenderBasis *render_basis, u64 time_us)
{
  b32 selected = false;
  if (cell->type != CELL_WALL)
  {

    V4 color;
    switch (cell->type)
    {
      case CELL_NULL:     color = (V4){1, 0.0, 0.0, 0.0};
        break;
      case CELL_START:    color = (V4){1, 0.7, 0.3, 0.3};
        break;
      case CELL_PATH:     color = (V4){1, 0.8, 0.8, 0.9};
        break;
      case CELL_WALL:     color = (V4){1, 0.0, 0.2, 0.0};
        break;
      case CELL_HOLE:     color = (V4){1, 0.0, 0.2, 0.7};
        break;
      case CELL_SPLITTER: color = (V4){1, 0.7, 0.6, 0.0};
        break;
      case CELL_FUNCTION: color = (V4){1, 0.7, 0.5, 0.3};
        break;
      case CELL_ONCE:     color = (V4){1, 0.7, 0.4, 0.0};
        break;
      case CELL_UP_UNLESS_DETECT:     color = (V4){1, 0.0, 0.1, 0.3};
        break;
      case CELL_DOWN_UNLESS_DETECT:   color = (V4){1, 0.0, 0.1, 0.3};
        break;
      case CELL_LEFT_UNLESS_DETECT:   color = (V4){1, 0.0, 0.1, 0.3};
        break;
      case CELL_RIGHT_UNLESS_DETECT:  color = (V4){1, 0.0, 0.1, 0.3};
        break;
      case CELL_OUT:      color = (V4){1, 0.0, 0.1, 0.3};
        break;
      case CELL_INP:      color = (V4){1, 0.0, 0.1, 0.3};
        break;
      case CELL_UP:       color = (V4){1, 0.0, 0.0, 0.0};
        break;
      case CELL_DOWN:     color = (V4){1, 0.3, 0.0, 0.0};
        break;
      case CELL_LEFT:     color = (V4){1, 0.0, 0.1, 0.3};
        break;
      case CELL_RIGHT:    color = (V4){1, 0.3, 0.1, 0.3};
        break;
      case CELL_PAUSE:    color = (V4){1, 0.3, 0.0, 0.7};
        break;
    }

    u32 cell_radius = (game_state->cell_spacing - (game_state->cell_spacing * game_state->cell_margin)) / 2;

    // NOTE: Tile centred on coord
    V2 world_pos = cell_coord_to_world(game_state, cell->x, cell->y);
    Rectangle cell_bounds = radius_rectangle(world_pos, cell_radius);

    if (cell->hovered_at_time == time_us)
    {
      selected = true;

      color.r = min(color.r + 0.15, 1.0f);
      color.g = min(color.g + 0.15, 1.0f);
      color.b = min(color.b + 0.15, 1.0f);
    }

    V2 scale = (render_basis->scale * cell_radius * 2 / game_state->world_per_pixel) / (V2){game_state->tile.file->width - 1, game_state->tile.file->height - 1};

    draw_box(frame_buffer, render_basis, cell_bounds, color);
  }

  return selected;
}


void
render_cells(GameState *game_state, Mouse *mouse, FrameBuffer *frame_buffer, RenderBasis *render_basis, QuadTree *tree, u64 time_us)
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
        selected |= render_cell(cell, game_state, mouse, frame_buffer, render_basis, time_us);
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

    render_cells(game_state, mouse, frame_buffer, render_basis, tree->top_right, time_us);
    render_cells(game_state, mouse, frame_buffer, render_basis, tree->top_left, time_us);
    render_cells(game_state, mouse, frame_buffer, render_basis, tree->bottom_right, time_us);
    render_cells(game_state, mouse, frame_buffer, render_basis, tree->bottom_left, time_us);
  }
}

