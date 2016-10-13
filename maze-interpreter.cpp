void
render(Memory *memory, GameState *game_state, FrameBuffer *frame_buffer, Rectangle render_region_pixels, Mouse *mouse, u64 time_us)
{
  // TODO: Give zoom velocity, to make it smooth

  game_state->d_zoom += mouse->scroll.y;

  if (game_state->inputs[ZOOM_IN].active)
  {
    game_state->d_zoom += .2;
  }
  if (game_state->inputs[ZOOM_OUT].active)
  {
    game_state->d_zoom -= .2;
  }

  r32 old_zoom = game_state->zoom;
  game_state->zoom += game_state->d_zoom;
  game_state->d_zoom *= 0.7f;

  if (game_state->d_zoom > 0 && ((game_state->zoom >= MAX_ZOOM) ||
                                 (game_state->zoom < old_zoom)))
  {
    game_state->d_zoom = 0;
    game_state->zoom = MAX_ZOOM;
  }
  else if (game_state->d_zoom < 0 && ((game_state->zoom <= MIN_ZOOM) ||
                                      (game_state->zoom > old_zoom)))
  {
    game_state->d_zoom = 0;
    game_state->zoom = MIN_ZOOM;
  }
  r32 scale = squared(game_state->zoom / 30.0f);

  V2 screen_mouse_pixels = (V2){mouse->x, mouse->y};
  V2 d_screen_mouse_pixels = screen_mouse_pixels - game_state->last_mouse_pos;

  game_state->last_mouse_pos = screen_mouse_pixels;

  if (mouse->l_down)
  {
    game_state->maze_pos += d_screen_mouse_pixels;
  }

  RenderBasis *render_basis = &game_state->last_frame_render_basis;
  render_basis->world_per_pixel = game_state->world_per_pixel;
  render_basis->scale = scale;
  render_basis->origin = game_state->maze_pos * game_state->world_per_pixel;
  render_basis->clip_region = render_region_pixels * game_state->world_per_pixel;

  if (abs(game_state->zoom - old_zoom) > 0.1)
  {
    render_basis->scale_focus = game_state->scale_focus;
    game_state->scale_focus = untransform_coord(render_basis, screen_mouse_pixels);
  }
  render_basis->scale_focus = game_state->scale_focus;

  render_cells(memory, game_state, mouse, frame_buffer, render_basis, &(game_state->maze.tree));
  render_cars(game_state, frame_buffer, render_basis, &(game_state->cars), time_us);
  // render_particles(&(game_state->particles), frame_buffer, render_basis);

  draw_formatted_string(frame_buffer, render_basis, &game_state->font, (V2){(game_state->maze.width) * game_state->cell_spacing}, ">: '%s'", game_state->persistent_str);

#if 0
  RenderBasis orthographic_basis;
  get_orthographic_basis(&orthographic_basis, frame_buffer);
  draw_circle(frame_buffer, &orthographic_basis, game_state->maze_pos, 10, (V4){1, 0, .5, 1});

  draw_circle(frame_buffer, render_basis, render_basis->scale_focus, 100000, (V4){1, 0, .5, 1});
#endif
}


b32
sim_tick(GameState *game_state, u64 time_us)
{
  b32 result = false;

  if (time_us >= game_state->last_sim_tick + (u32)(seconds_in_u(1) / game_state->sim_ticks_per_s))
  {
    if (game_state->single_step)
    {
      if (game_state->inputs[STEP].active)
      {
        game_state->last_sim_tick = time_us;
        result = true;
      }
    }
    else
    {
      game_state->last_sim_tick = time_us;
      result = true;
    }
  }

  return result;
}


void
reset_zoom(GameState *game_state, FrameBuffer *frame_buffer)
{
  // TODO: Should world coords be r32s now we are using u32s for
  //       the cell position?
  game_state->world_per_pixel = 64*64;
  game_state->cell_spacing = 100000;
  game_state->cell_margin = 0.2f;
  game_state->last_mouse_pos = (V2){0};

  // NOTE: Somewhere between the sqrt( [ MIN, MAX ]_WORLD_PER_PIXEL )
  game_state->zoom = 30;

  V2 screen_size = (V2){frame_buffer->width, frame_buffer->height};
  V2 maze_size_in_pixels = (V2){game_state->maze.width, game_state->maze.height} * game_state->cell_spacing / game_state->world_per_pixel;
  game_state->maze_pos = (0.5f * screen_size) - (0.5f * maze_size_in_pixels);
}


void
load_maze(Memory *memory, GameState *game_state, u32 argc, char *argv[])
{
  if (argc > 1)
  {
    parse(&(game_state->maze), memory, argv[1]);
  }
  else
  {
    parse(&(game_state->maze), memory, MAZE_FILENAME);
  }

  delete_all_cars(&(game_state->cars));

  game_state->last_sim_tick = 0;
  game_state->sim_steps = 0;
}


void
init_game(Memory *memory, GameState *game_state, Keys *keys, u64 time_us, u32 argc, char *argv[])
{
  game_state->init = true;

  game_state->single_step = false;
  game_state->sim_ticks_per_s = 5;

  setup_inputs(keys, game_state->inputs);

  load_bitmap(&game_state->particles.spark_bitmap, "particles/spark.bmp");
  load_bitmap(&game_state->particles.cross_bitmap, "particles/cross.bmp");
  load_bitmap(&game_state->particles.blob_bitmap, "particles/blob.bmp");
  load_bitmap(&game_state->particles.smoke_bitmap, "particles/smoke.bmp");
  load_bitmap(&game_state->tile, "tile.bmp");
  load_bitmap(&game_state->font, "font.bmp");

  strcpy(game_state->persistent_str, "Init!");
}


void
update_and_render(Memory *memory, GameState *game_state, FrameBuffer *frame_buffer, Keys *keys, Mouse *mouse, u64 time_us, u32 last_frame_dt, u32 argc, char *argv[])
{
  if (!game_state->init)
  {
    init_game(memory, game_state, keys, time_us, argc, argv);
    load_maze(memory, game_state, argc, argv);
    reset_zoom(game_state, frame_buffer);
  }

  update_inputs(keys, game_state->inputs, time_us);

  if (game_state->inputs[RELOAD].active)
  {
    strcpy(game_state->persistent_str, "Reload!");
    load_maze(memory, game_state, argc, argv);
  }

  if (game_state->inputs[RESET].active)
  {
    strcpy(game_state->persistent_str, "Reset!");
    reset_zoom(game_state, frame_buffer);
  }

  if (game_state->inputs[RESTART].active)
  {
    strcpy(game_state->persistent_str, "Restart!");
    delete_all_cars(&(game_state->cars));
    game_state->last_sim_tick = 0;
    game_state->sim_steps = 0;
  }

  if (game_state->inputs[STEP_MODE_TOGGLE].active)
  {
    game_state->single_step = !game_state->single_step;
    log(L_GameLoop, "Changing stepping mode");
  }

  if (game_state->inputs[SIM_TICKS_INC].active)
  {
    game_state->sim_ticks_per_s += .5f;
  }
  if (game_state->inputs[SIM_TICKS_DEC].active)
  {
    game_state->sim_ticks_per_s -= .5f;
  }
  game_state->sim_ticks_per_s = clamp(.5, game_state->sim_ticks_per_s, 20);

  update_cells_ui_state(game_state, mouse, time_us);

  if (sim_tick(game_state, time_us))
  {
    perform_cells_sim_tick(memory, game_state, &(game_state->maze.tree), time_us);
    perform_cars_sim_tick(memory, game_state, time_us);

    ++game_state->sim_steps;
  }

  annimate_cars(game_state, last_frame_dt);
  step_particles(&(game_state->particles), time_us);

  Rectangle render_region_pixels;
  render_region_pixels.start = (V2){0, 0};
  render_region_pixels.end = (V2){frame_buffer->width, frame_buffer->height};

  RenderBasis orthographic_basis;
  get_orthographic_basis(&orthographic_basis, frame_buffer);

  fast_draw_box(frame_buffer, &orthographic_basis, render_region_pixels, (PixelColor){255, 255, 255});

  render(memory, game_state, frame_buffer, render_region_pixels, mouse, time_us);
}