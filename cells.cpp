void
update_cells_ui_state(GameState *game_state, RenderBasis *render_basis, Mouse *mouse, u64 time_us)
{
  V2 mouse_maze_pos = untransform_coord(render_basis, (V2){mouse->x, mouse->y});
  V2 mouse_cell_pos = round_down((mouse_maze_pos / game_state->cell_spacing) + 0.5f);

  Cell *cell_hovered_over = get_cell(&game_state->maze, mouse_cell_pos.x, mouse_cell_pos.y);
  if (cell_hovered_over && cell_hovered_over->type != CELL_NULL)
  {
    cell_hovered_over->hovered_at_time = time_us;
  }

  if (mouse->l_on_up && !game_state->panning_this_frame)
  {
    if (cell_hovered_over)
    {
      if (time_us >= cell_hovered_over->edit_mode_last_change + seconds_in_u(0.01))
      {
        cell_hovered_over->edit_mode_last_change = time_us;

        // Only update time if the menu wasn't already open
        if (game_state->ui.cell_type_menu.cell == 0)
        {
          game_state->ui.cell_type_menu.opened_on_frame = time_us;
        }
        game_state->ui.cell_type_menu.cell = cell_hovered_over;
      }
    }
    else
    {
      game_state->ui.cell_type_menu.cell = 0;
    }
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
