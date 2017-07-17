u32
calc_cell_radius(GameState *game_state)
{
  return 1 - (game_state->cell_margin * 0.5);
}


void
update_cells_ui_state(GameState *game_state, Mouse *mouse, WorldSpace _mouse_pos, u64 time_us)
{
  vec2 cell_pos;
  vec2 mouse_pos;
  Rectangle cell_bounds = radius_rectangle(cell_pos, calc_cell_radius(game_state));

  b32 hide_cell_menu = false;
  b32 mouse_click = mouse->l_on_up;

  if (in_rectangle(mouse_pos, cell_bounds))
  {
    Cell *cell_hovered_over = get_cell(&game_state->maze, cell_pos.x, cell_pos.y);
    if (cell_hovered_over && cell_hovered_over->type != CELL_NULL)
    {
      cell_hovered_over->hovered_at_time = time_us;
    }

    if (mouse_click)
    {
      if (cell_hovered_over)
      {
        if (time_us >= cell_hovered_over->edit_mode_last_change + seconds_in_u(0.01))
        {
          cell_hovered_over->edit_mode_last_change = time_us;

          open_cell_type_menu(game_state, cell_hovered_over, time_us);
        }
      }
      else
      {
        hide_cell_menu = true;
      }
    }
  }
  else if (mouse_click)
  {
    hide_cell_menu = true;
  }

  if (hide_cell_menu)
  {
    game_state->ui.cell_type_menu.cell = 0;
  }
}


void
perform_cells_sim_tick(Memory *memory, GameState *game_state, QuadTree *tree, u64 time_us)
{
  if (game_state->sim_steps == 0)
  {
    if (tree)
    {
      for (u32 cell_index = 0;
           cell_index < tree->used;
           ++cell_index)
      {
        Cell *cell = tree->cells + cell_index;

        if (cell->type == CELL_START)
        {
          Car *new_car = get_new_car(memory, &game_state->cars);
          init_car(game_state, time_us, new_car, cell->x, cell->y);
        }
      }

      perform_cells_sim_tick(memory, game_state, tree->top_right, time_us);
      perform_cells_sim_tick(memory, game_state, tree->top_left, time_us);
      perform_cells_sim_tick(memory, game_state, tree->bottom_right, time_us);
      perform_cells_sim_tick(memory, game_state, tree->bottom_left, time_us);
    }
  }
}


void
load_cell_bitmaps(CellBitmaps *cell_bitmaps)
{
  load_bitmap(&cell_bitmaps->walkable[DISP_TYPE_ENCLOSED], "cells/path-enclosed.bmp");
  load_bitmap(&cell_bitmaps->walkable[DISP_TYPE_SINGLE],   "cells/path-single.bmp");
  load_bitmap(&cell_bitmaps->walkable[DISP_TYPE_L],        "cells/path-l.bmp");
  load_bitmap(&cell_bitmaps->walkable[DISP_TYPE_STRAIGHT], "cells/path-straight.bmp");
  load_bitmap(&cell_bitmaps->walkable[DISP_TYPE_T],        "cells/path-t.bmp");
  load_bitmap(&cell_bitmaps->walkable[DISP_TYPE_CROSS],    "cells/path-cross.bmp");
  load_bitmap(&cell_bitmaps->unwalkable[DISP_TYPE_ENCLOSED], "cells/unwalkable-enclosed.bmp");
  load_bitmap(&cell_bitmaps->unwalkable[DISP_TYPE_SINGLE],   "cells/unwalkable-single.bmp");
  load_bitmap(&cell_bitmaps->unwalkable[DISP_TYPE_L],        "cells/unwalkable-l.bmp");
  load_bitmap(&cell_bitmaps->unwalkable[DISP_TYPE_STRAIGHT], "cells/unwalkable-straight.bmp");
  load_bitmap(&cell_bitmaps->unwalkable[DISP_TYPE_T],        "cells/unwalkable-t.bmp");
  load_bitmap(&cell_bitmaps->unwalkable[DISP_TYPE_CROSS],    "cells/unwalkable-cross.bmp");

  load_bitmap(&cell_bitmaps->arrow, "cells/arrow.bmp");
  load_bitmap(&cell_bitmaps->splitter, "cells/splitter.bmp");
}


void
directly_neighbouring_cells(Cell *neighbours[4], Maze *maze, u32 cell_x, u32 cell_y)
{
  neighbours[0] = get_cell(maze, cell_x, cell_y-1);
  neighbours[1] = get_cell(maze, cell_x, cell_y+1);
  neighbours[2] = get_cell(maze, cell_x+1, cell_y);
  neighbours[3] = get_cell(maze, cell_x-1, cell_y);
}


CellConnectedState
cell_walkable(CellType type)
{
  CellConnectedState result = WALKABLE;

  if (type == CELL_WALL)
  {
    result = UNWALKABLE;
  }
  else if (type == CELL_NULL)
  {
    result = UNCONNECTED;
  }

  return result;
}


CellConnectedState
cell_walkable(Cell *cell)
{
  CellConnectedState result = UNCONNECTED;

  if (cell)
  {
    result = cell_walkable(cell->type);
  }

  return result;
}


vec4
get_cell_color(CellType type)
{
  vec4 result;
  switch (type)
  {
    case CELL_NULL:     result = (vec4){1, 0.0, 0.0, 0.0};
      break;
    case CELL_START:    result = (vec4){1, 0.7, 0.3, 0.3};
      break;
    case CELL_PATH:     result = (vec4){1, 0.8, 0.8, 0.9};
      break;
    case CELL_WALL:     result = (vec4){1, 0.9, 0.8, 0.9};
      break;
    case CELL_HOLE:     result = (vec4){1, 0.0, 0.2, 0.7};
      break;
    case CELL_SPLITTER: result = (vec4){1, 0.7, 0.6, 0.0};
      break;
    case CELL_FUNCTION: result = (vec4){1, 0.7, 0.5, 0.3};
      break;
    case CELL_ONCE:     result = (vec4){1, 0.7, 0.4, 0.0};
      break;
    case CELL_UP_UNLESS_DETECT:     result = (vec4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_DOWN_UNLESS_DETECT:   result = (vec4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_LEFT_UNLESS_DETECT:   result = (vec4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_RIGHT_UNLESS_DETECT:  result = (vec4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_OUT:      result = (vec4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_INP:      result = (vec4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_UP:       result = (vec4){1, 0.0, 0.0, 0.0};
      break;
    case CELL_DOWN:     result = (vec4){1, 0.3, 0.0, 0.0};
      break;
    case CELL_LEFT:     result = (vec4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_RIGHT:    result = (vec4){1, 0.3, 0.1, 0.3};
      break;
    case CELL_PAUSE:    result = (vec4){1, 0.3, 0.0, 0.7};
      break;

    default:
      invalid_code_path;
      break;
  }

  return result;
}


void
calc_connected_cell_bitmap(Maze *maze, Cell *cell, CellBitmaps *cell_bitmaps, CellDisplay *cell_display_result)
{
  cell_display_result->rotate = 0;
  CellConnectedState walkable = cell_walkable(cell);

  Cell *n[] = {0, 0, 0, 0};
  directly_neighbouring_cells(n, maze, cell->x, cell->y);

  CellDisplayType cell_display_type;
  if      (walkable == cell_walkable(n[0]) && walkable == cell_walkable(n[1]) &&
           walkable == cell_walkable(n[2]) && walkable == cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_CROSS;
    cell_display_result->rotate = 0;
  }
  else if (walkable == cell_walkable(n[0]) && walkable == cell_walkable(n[1]) &&
           walkable == cell_walkable(n[2]) && walkable != cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_T;
    cell_display_result->rotate = 90;
  }
  else if (walkable == cell_walkable(n[0]) && walkable == cell_walkable(n[1]) &&
           walkable != cell_walkable(n[2]) && walkable == cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_T;
    cell_display_result->rotate = 270;
  }
  else if (walkable == cell_walkable(n[0]) && walkable != cell_walkable(n[1]) &&
           walkable == cell_walkable(n[2]) && walkable == cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_T;
    cell_display_result->rotate = 0;
  }
  else if (walkable != cell_walkable(n[0]) && walkable == cell_walkable(n[1]) &&
           walkable == cell_walkable(n[2]) && walkable == cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_T;
    cell_display_result->rotate = 180;
  }
  else if (walkable == cell_walkable(n[0]) && walkable == cell_walkable(n[1]) &&
           walkable != cell_walkable(n[2]) && walkable != cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_STRAIGHT;
    cell_display_result->rotate = 0;
  }
  else if (walkable != cell_walkable(n[0]) && walkable != cell_walkable(n[1]) &&
           walkable == cell_walkable(n[2]) && walkable == cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_STRAIGHT;
    cell_display_result->rotate = 90;
  }
  else if (walkable == cell_walkable(n[0]) && walkable != cell_walkable(n[1]) &&
           walkable == cell_walkable(n[2]) && walkable != cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_L;
    cell_display_result->rotate = 0;
  }
  else if (walkable == cell_walkable(n[0]) && walkable != cell_walkable(n[1]) &&
           walkable != cell_walkable(n[2]) && walkable == cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_L;
    cell_display_result->rotate = 270;
  }
  else if (walkable != cell_walkable(n[0]) && walkable == cell_walkable(n[1]) &&
           walkable == cell_walkable(n[2]) && walkable != cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_L;
    cell_display_result->rotate = 90;
  }
  else if (walkable != cell_walkable(n[0]) && walkable == cell_walkable(n[1]) &&
           walkable != cell_walkable(n[2]) && walkable == cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_L;
    cell_display_result->rotate = 180;
  }
  else if (walkable == cell_walkable(n[0]) && walkable != cell_walkable(n[1]) &&
           walkable != cell_walkable(n[2]) && walkable != cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_SINGLE;
    cell_display_result->rotate = 0;
  }
  else if (walkable != cell_walkable(n[0]) && walkable == cell_walkable(n[1]) &&
           walkable != cell_walkable(n[2]) && walkable != cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_SINGLE;
    cell_display_result->rotate = 180;
  }
  else if (walkable != cell_walkable(n[0]) && walkable != cell_walkable(n[1]) &&
           walkable == cell_walkable(n[2]) && walkable != cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_SINGLE;
    cell_display_result->rotate = 90;
  }
  else if (walkable != cell_walkable(n[0]) && walkable != cell_walkable(n[1]) &&
           walkable != cell_walkable(n[2]) && walkable == cell_walkable(n[3]))
  {
    cell_display_type = DISP_TYPE_SINGLE;
    cell_display_result->rotate = 270;
  }
  else
  {
    cell_display_type = DISP_TYPE_ENCLOSED;
  }

  if (walkable == WALKABLE)
  {
    cell_display_result->bitmap = &cell_bitmaps->walkable[cell_display_type];
  }
  else
  {
    cell_display_result->bitmap = &cell_bitmaps->unwalkable[cell_display_type];
  }
}


vec2
get_direction_cell_direction(CellType type)
{
  vec2 result = {0, 0};

  switch (type)
  {
    case CELL_UP:
    case CELL_UP_UNLESS_DETECT:
      result = UP;
      break;
    case CELL_DOWN:
    case CELL_DOWN_UNLESS_DETECT:
      result = DOWN;
      break;
    case CELL_LEFT:
    case CELL_LEFT_UNLESS_DETECT:
      result = LEFT;
      break;
    case CELL_RIGHT:
    case CELL_RIGHT_UNLESS_DETECT:
      result = RIGHT;
      break;

    default: break;
  }

  return result;
}


void
get_overlay_display(CellType type, CellBitmaps *cell_bitmaps, CellDisplay *overlay_display_result)
{
  vec2 cell_direction = get_direction_cell_direction(type);
  if (cell_direction != (vec2){0, 0})
  {
    overlay_display_result->bitmap = &cell_bitmaps->arrow;
    overlay_display_result->rotate = angle_from_vector(cell_direction);

    if (type == CELL_UP_UNLESS_DETECT ||
        type == CELL_DOWN_UNLESS_DETECT ||
        type == CELL_LEFT_UNLESS_DETECT ||
        type == CELL_RIGHT_UNLESS_DETECT)
    {
      overlay_display_result->color = (vec4){1, 1, 0, 0};
    }
    else
    {
      overlay_display_result->color = (vec4){1, 1, 1, 1};
    }
  }
  else if (type == CELL_SPLITTER)
  {
    overlay_display_result->color = (vec4){1, 1, 1, 1};
    overlay_display_result->bitmap = &cell_bitmaps->splitter;
  }
  else
  {
    overlay_display_result->bitmap = 0;
  }
}


void
draw_cell(CellType type, vec2 normalised_world_pos, u32 cell_radius, b32 hovered, CellBitmaps *cell_bitmaps, CellDisplay *cell_display)
{
  Bitmap *cell_bitmap;
  u32 rotate = 0;
  if (cell_display)
  {
    cell_bitmap = cell_display->bitmap;
    rotate = cell_display->rotate;
  }
  else
  {
    if (cell_walkable(type) == WALKABLE)
    {
      cell_bitmap = &cell_bitmaps->walkable[DISP_TYPE_CROSS];
    }
    else
    {
      cell_bitmap = &cell_bitmaps->unwalkable[DISP_TYPE_CROSS];
    }
  }

  vec4 color = get_cell_color(type);
  if (hovered)
  {
    color.r = min(color.r + 0.15, 1.0f);
    color.g = min(color.g + 0.15, 1.0f);
    color.b = min(color.b + 0.15, 1.0f);
  }

  CellDisplay overlay_display;
  get_overlay_display(type, cell_bitmaps, &overlay_display);

  glPushMatrix();
    glTranslatef(normalised_world_pos.x, normalised_world_pos.y, 0);
    glColor3f(color.r, color.g, color.b);

    draw_box();
  glPopMatrix();

  normalised_world_pos -= cell_radius;
  // BlitBitmapOptions opts;
  // get_default_blit_bitmap_options(&opts);
  // opts.scale = 2.0*cell_radius / (r32)cell_bitmap->file->width;
  // opts.interpolation = false;

  // opts.color_multiplier = color;
  // opts.rotate = rotate;
  // draw_bitmap(cell_bitmap, normalised_world_pos, &opts);

  // if (overlay_display.bitmap)
  // {
  //   opts.rotate = overlay_display.rotate;
  //   opts.color_multiplier = overlay_display.color;
  //   opts.scale = 2.0*cell_radius / (r32)overlay_display.bitmap->file->width;
  //   draw_bitmap(overlay_display.bitmap, normalised_world_pos, &opts);
  // }
}


void
recursively_draw_cells(GameState *game_state, RenderWindow *render_window, QuadTree *tree, u32 cell_radius, u64 time_us)
{
  b32 on_screen = false;

  // Rectangle cell_clip_region_pixels = (Rectangle){render_basis->clip_region.start, render_basis->clip_region.end} / render_basis->world_per_pixel;

  if (tree)
  {
    // TODO: Update this to work with opengl
    // on_screen = overlaps(cell_clip_region_pixels, transform_coord_rect(render_basis, grow(tree->bounds, .5) * game_state->cell_spacing));
    on_screen = true;

    if (on_screen)
    {
      for (u32 cell_index = 0;
           cell_index < tree->used;
           ++cell_index)
      {
        Cell *cell = tree->cells + cell_index;

        vec2 normalised_cell_pos = world_coord_to_render_window_coord(render_window, cell->x, cell->y);

        CellDisplay cell_display;
        calc_connected_cell_bitmap(&game_state->maze, cell, &game_state->cell_bitmaps, &cell_display);

        draw_cell(cell->type, normalised_cell_pos, cell_radius, cell->hovered_at_time == time_us, &game_state->cell_bitmaps, &cell_display);
      }
    }

    recursively_draw_cells(game_state, render_window, tree->top_right, cell_radius, time_us);
    recursively_draw_cells(game_state, render_window, tree->top_left, cell_radius, time_us);
    recursively_draw_cells(game_state, render_window, tree->bottom_right, cell_radius, time_us);
    recursively_draw_cells(game_state, render_window, tree->bottom_left, cell_radius, time_us);
  }
}


void
recursively_draw_tree_blocks(GameState *game_state, RenderWindow *render_window, QuadTree *tree)
{
  // Low-res cell drawing
  // TODO: Do we still need this with OpenGL rendering?

  if (tree)
  {
    vec4 avg_color = {0, 0, 0, 0};
    Rectangle normalised_bounds = {{0, 0}, {0, 0}};
    for (u32 cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell *cell = tree->cells + cell_index;
      avg_color += get_cell_color(cell->type);

      vec2 normalised_pos = world_coord_to_render_window_coord(render_window, cell->x, cell->y);

      if (normalised_pos.x < normalised_bounds.start.x)
      {
        normalised_bounds.start.x = normalised_pos.x;
      }
      if (normalised_pos.y < normalised_bounds.start.y)
      {
        normalised_bounds.start.y = normalised_pos.y;
      }
      if (normalised_pos.x > normalised_bounds.end.x)
      {
        normalised_bounds.end.x = normalised_pos.x;
      }
      if (normalised_pos.y > normalised_bounds.end.y)
      {
        normalised_bounds.end.y = normalised_pos.y;
      }
    }
    avg_color /= tree->used;

    // TODO: Update this to work with OpenGL
    // b32 on_screen = overlaps(render_basis->clip_region,  transform_coord_rect(render_basis, tree->bounds*game_state->cell_spacing));
    b32 on_screen = true;

    if (on_screen)
    {
      draw_box(normalised_bounds, avg_color);
    }

    recursively_draw_tree_blocks(game_state, render_window, tree->top_right);
    recursively_draw_tree_blocks(game_state, render_window, tree->top_left);
    recursively_draw_tree_blocks(game_state, render_window, tree->bottom_right);
    recursively_draw_tree_blocks(game_state, render_window, tree->bottom_left);
  }
}


void
draw_cells(GameState *game_state, RenderWindow *render_window, QuadTree *tree, u64 time_us)
{
  // TODO:
  // if (render_basis->scale >= 0.01)
  if (1)
  {
    u32 cell_radius = calc_cell_radius(game_state);

    recursively_draw_cells(game_state, render_window, tree, cell_radius, time_us);

    // Animate highlight for cell which is currently being edited
    if (game_state->ui.cell_type_menu.cell)
    {
      if (game_state->ui.cell_type_menu.highlighted_cell_annimation_offset.x == -1)
      {
        // No animation if there was no previously highlighted cell
        game_state->ui.cell_type_menu.highlighted_cell_annimation_offset = (vec2){0, 0};
      }
      else
      {
        // TODO: Proper animation
        game_state->ui.cell_type_menu.highlighted_cell_annimation_offset *= 0.3;
      }

      WorldSpace highlight_world_pos = {
        game_state->ui.cell_type_menu.cell->x,
        game_state->ui.cell_type_menu.cell->y,
        game_state->ui.cell_type_menu.highlighted_cell_annimation_offset
      };

      re_form_world_coord(&highlight_world_pos);
      vec2 normalised_highlight_pos = world_coord_to_render_window_coord(render_window, highlight_world_pos);

      glPushMatrix();
        glColor3f(.9, .1, .2); // TODO: Alpha: 0.3
        glTranslatef(normalised_highlight_pos.x, normalised_highlight_pos.y, 0);

        draw_box_outline(Vec2(cell_radius, cell_radius), 0.3);
      glPopMatrix();
    }
    else
    {
      game_state->ui.cell_type_menu.highlighted_cell_annimation_offset = (vec2){-1, -1};
    }
  }
  else
  {
    recursively_draw_tree_blocks(game_state, render_window, tree);
  }
}