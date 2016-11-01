V2
cell_coord_to_world(u32 cell_spacing, u32 cell_x, u32 cell_y)
{
  // TODO: Is there any point in world space any more???
  //       Cars have cell_x/y and offset, is that all we need? (Help
  //       with precision?)
  V2 result = ((V2){cell_x, cell_y} * cell_spacing);
  return result;
}


u32
calc_cell_radius(GameState *game_state)
{
  return (game_state->cell_spacing - (game_state->cell_spacing * game_state->cell_margin)) * 0.5;
}


void
update_cells_ui_state(GameState *game_state, RenderBasis *render_basis, Mouse *mouse, u64 time_us)
{
  V2 mouse_maze_pos = untransform_coord(render_basis, (V2){mouse->x, mouse->y});
  V2 cell_pos = round_down((mouse_maze_pos / game_state->cell_spacing) + 0.5f);

  V2 world_cell_pos = cell_coord_to_world(game_state->cell_spacing, cell_pos.x, cell_pos.y);
  Rectangle cell_bounds = radius_rectangle(world_cell_pos, calc_cell_radius(game_state));

  b32 hide_cell_menu = false;
  b32 mouse_click = mouse->l_on_up;

  if (in_rectangle(mouse_maze_pos, cell_bounds))
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
}


void
directly_neighbouring_cells(Cell *neighbours[4], Maze *maze, u32 cell_x, u32 cell_y)
{
  neighbours[0] = get_cell(maze, cell_x, cell_y-1);
  neighbours[1] = get_cell(maze, cell_x, cell_y+1);
  neighbours[2] = get_cell(maze, cell_x+1, cell_y);
  neighbours[3] = get_cell(maze, cell_x-1, cell_y);
}


b32
cell_walkable(CellType type)
{
  b32 result = true;

  if (type == CELL_NULL || type == CELL_WALL)
  {
    result = false;
  }

  return result;
}


b32
cell_walkable(Cell *cell)
{
  b32 result = false;

  if (cell)
  {
    result = cell_walkable(cell->type);
  }

  return result;
}


V4
get_cell_color(CellType type)
{
  V4 result;
  switch (type)
  {
    case CELL_NULL:     result = (V4){1, 0.0, 0.0, 0.0};
      break;
    case CELL_START:    result = (V4){1, 0.7, 0.3, 0.3};
      break;
    case CELL_PATH:     result = (V4){1, 0.8, 0.8, 0.9};
      break;
    case CELL_WALL:     result = (V4){1, 0.9, 0.8, 0.9};
      break;
    case CELL_HOLE:     result = (V4){1, 0.0, 0.2, 0.7};
      break;
    case CELL_SPLITTER: result = (V4){1, 0.7, 0.6, 0.0};
      break;
    case CELL_FUNCTION: result = (V4){1, 0.7, 0.5, 0.3};
      break;
    case CELL_ONCE:     result = (V4){1, 0.7, 0.4, 0.0};
      break;
    case CELL_UP_UNLESS_DETECT:     result = (V4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_DOWN_UNLESS_DETECT:   result = (V4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_LEFT_UNLESS_DETECT:   result = (V4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_RIGHT_UNLESS_DETECT:  result = (V4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_OUT:      result = (V4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_INP:      result = (V4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_UP:       result = (V4){1, 0.0, 0.0, 0.0};
      break;
    case CELL_DOWN:     result = (V4){1, 0.3, 0.0, 0.0};
      break;
    case CELL_LEFT:     result = (V4){1, 0.0, 0.1, 0.3};
      break;
    case CELL_RIGHT:    result = (V4){1, 0.3, 0.1, 0.3};
      break;
    case CELL_PAUSE:    result = (V4){1, 0.3, 0.0, 0.7};
      break;

    default:
      invalid_code_path;
      break;
  }

  return result;
}


void
get_cell_bitmap(Maze *maze, Cell *cell, CellBitmaps *cell_bitmaps, CellDisplay *cell_display_result)
{
  cell_display_result->rotate = 0;
  b32 walkable = cell_walkable(cell);

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

  if (walkable)
  {
    cell_display_result->bitmap = &cell_bitmaps->walkable[cell_display_type];
  }
  else
  {
    cell_display_result->bitmap = &cell_bitmaps->unwalkable[cell_display_type];
  }
}


void
draw_cell(RenderOperations *render_operations, RenderBasis *render_basis, CellType type, V2 world_pos, u32 cell_radius, b32 hovered, CellBitmaps *cell_bitmaps, CellDisplay *cell_display)
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
    if (cell_walkable(type))
    {
      cell_bitmap = &cell_bitmaps->walkable[DISP_TYPE_CROSS];
    }
    else
    {
      cell_bitmap = &cell_bitmaps->unwalkable[DISP_TYPE_CROSS];
    }
  }

  V4 color = get_cell_color(type);
  if (hovered)
  {
    color.r = min(color.r + 0.15, 1.0f);
    color.g = min(color.g + 0.15, 1.0f);
    color.b = min(color.b + 0.15, 1.0f);
  }

  V2 bitmap_pos = world_pos - cell_radius;

  BlitBitmapOptions opts;
  get_default_blit_bitmap_options(&opts);
  opts.scale = 2.0*cell_radius / (render_basis->world_per_pixel*(r32)cell_bitmap->file->width);
  opts.interpolation = false;
  // opts.invert = true;
  opts.rotate = rotate;
  opts.color_multiplier = color;

  add_bitmap_to_render_list(render_operations, render_basis, cell_bitmap, bitmap_pos, &opts);
}


void
recursively_draw_cells(GameState *game_state, RenderOperations *render_operations, RenderBasis *render_basis, QuadTree *tree, u32 cell_radius, u64 time_us)
{
  b32 on_screen = false;

  Rectangle cell_clip_region_pixels = (Rectangle){render_basis->clip_region.start, render_basis->clip_region.end} / render_basis->world_per_pixel;

  if (tree)
  {
    on_screen = overlaps(cell_clip_region_pixels, transform_coord_rect(render_basis, grow(tree->bounds, .5) * game_state->cell_spacing));

    if (on_screen)
    {
      for (u32 cell_index = 0;
           cell_index < tree->used;
           ++cell_index)
      {
        Cell *cell = tree->cells + cell_index;

        V2 world_pos = cell_coord_to_world(game_state->cell_spacing, cell->x, cell->y);

        CellDisplay cell_display;
        get_cell_bitmap(&game_state->maze, cell, &game_state->cell_bitmaps, &cell_display);
        draw_cell(render_operations, render_basis, cell->type, world_pos, cell_radius, cell->hovered_at_time == time_us, &game_state->cell_bitmaps, &cell_display);
      }
    }

#if 0
    V4 box_color = (V4){0.5f, 0, 0, 0};
    Rectangle world_tree_bounds = (tree->bounds * game_state->cell_spacing);
    if (on_screen)
    {
      box_color.a = 1;
      box_color.r = 1;
    }
    RenderBasis tmp_basis = *render_basis;
    tmp_basis.clip_region = grow(tmp_basis.clip_region, 0);

    add_box_outline_to_render_list(render_operations, &tmp_basis, world_tree_bounds, box_color);
#endif

    recursively_draw_cells(game_state, render_operations, render_basis, tree->top_right, cell_radius, time_us);
    recursively_draw_cells(game_state, render_operations, render_basis, tree->top_left, cell_radius, time_us);
    recursively_draw_cells(game_state, render_operations, render_basis, tree->bottom_right, cell_radius, time_us);
    recursively_draw_cells(game_state, render_operations, render_basis, tree->bottom_left, cell_radius, time_us);
  }
}


void
recursively_draw_tree_blocks(GameState *game_state, RenderOperations *render_operations, RenderBasis *render_basis, QuadTree *tree)
{

  Rectangle cell_clip_region_pixels = (Rectangle){render_basis->clip_region.start, render_basis->clip_region.end};

  if (tree)
  {
    V4 avg_color = {0, 0, 0, 0};
    Rectangle bounds = {{0, 0}, {0, 0}};
    for (u32 cell_index = 0;
         cell_index < tree->used;
         ++cell_index)
    {
      Cell *cell = tree->cells + cell_index;
      avg_color += get_cell_color(cell->type);

      if (cell->x < bounds.start.x)
      {
        bounds.start.x = cell->x;
      }
      if (cell->y < bounds.start.y)
      {
        bounds.start.y = cell->y;
      }
      if (cell->x > bounds.end.x)
      {
        bounds.end.x = cell->x;
      }
      if (cell->y > bounds.end.y)
      {
        bounds.end.y = cell->y;
      }
    }
    avg_color /= tree->used;

    b32 on_screen = overlaps(render_basis->clip_region,  transform_coord_rect(render_basis, tree->bounds*game_state->cell_spacing));

    if (on_screen)
    {
      log(L_Render, "Oh, shit");
      add_box_to_render_list(render_operations, render_basis, bounds*game_state->cell_spacing, avg_color);

    }
    recursively_draw_tree_blocks(game_state, render_operations, render_basis, tree->top_right);
    recursively_draw_tree_blocks(game_state, render_operations, render_basis, tree->top_left);
    recursively_draw_tree_blocks(game_state, render_operations, render_basis, tree->bottom_right);
    recursively_draw_tree_blocks(game_state, render_operations, render_basis, tree->bottom_left);
  }
}


void
draw_cells(GameState *game_state, RenderOperations *render_operations, RenderBasis *render_basis, QuadTree *tree, u64 time_us)
{
  if (render_basis->scale >= 0.01)
  {
    u32 cell_radius = calc_cell_radius(game_state);

    recursively_draw_cells(game_state, render_operations, render_basis, tree, cell_radius, time_us);

    if (game_state->ui.cell_type_menu.cell)
    {
      V2 target_world_pos = cell_coord_to_world(game_state->cell_spacing, game_state->ui.cell_type_menu.cell->x, game_state->ui.cell_type_menu.cell->y);

      V2 world_pos;
      if (game_state->ui.cell_type_menu.annimated_highlighted_cell_pos.x == -1)
      {
        game_state->ui.cell_type_menu.annimated_highlighted_cell_pos = target_world_pos;
      }
      else
      {
        V2 d_target = target_world_pos - game_state->ui.cell_type_menu.annimated_highlighted_cell_pos;
        game_state->ui.cell_type_menu.annimated_highlighted_cell_pos += d_target * 0.3;
      }

      Rectangle cell_bounds = radius_rectangle(game_state->ui.cell_type_menu.annimated_highlighted_cell_pos, cell_radius);

      V4 highlight_color = {0.3, .9, .1, .2};
      add_box_outline_to_render_list(render_operations, render_basis, cell_bounds, highlight_color, (s32)cell_radius*0.3);
    }
    else
    {
      game_state->ui.cell_type_menu.annimated_highlighted_cell_pos = (V2){-1, -1};
    }
  }
  else
  {
    recursively_draw_tree_blocks(game_state, render_operations, render_basis, tree);
  }
}