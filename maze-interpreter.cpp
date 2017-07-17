vec2
screen_pixels_to_screen_space(vec2 screen_size, vec2 screen_pixels)
{
  // Convert from top-left = (0, 0) bottom-right = (width, height)
  //         to top-left = (-1, -1) bottom-right = (1, 1)
  vec2 result = (2*screen_pixels / screen_size.y) - 1;
  return result;
}


void
update_pan_and_zoom(Panning *panning, Inputs *inputs, Mouse *mouse, vec2 screen_size)
{
  vec2 screen_mouse_pixels = screen_pixels_to_screen_space(screen_size, Vec2(mouse->x, mouse->y));

  r32 old_zoom_multiplier = panning->zoom_multiplier;
  panning->zoom_multiplier -= mouse->scroll.y * 0.01;

  if (inputs->maps[ZOOM_IN].active)
  {
    panning->zoom_multiplier += .2;
  }
  if (inputs->maps[ZOOM_OUT].active)
  {
    panning->zoom_multiplier -= .2;
  }

  // If just started zooming: use this mouse position for scale focus
  // if (old_zoom_multiplier < 0.0005 && panning->zoom_multiplier > 0.001)
  {
    panning->scale_focus_pixels = screen_mouse_pixels;
  }

  panning->old_zoom = panning->zoom;
  panning->zoom *= 1 + panning->zoom_multiplier;
  panning->zoom = max(0.005, min(1.0, panning->zoom));
  panning->zoom_multiplier *= 0.9f;

  vec2 d_screen_mouse_pixels = screen_mouse_pixels - panning->last_mouse_pos;
  panning->last_mouse_pos = screen_mouse_pixels;

  // TODO: Need a layer to filter whether the mouse is focused on the world
  if (mouse->l_down)
  {
    panning->world_maze_pos.offset -= d_screen_mouse_pixels / panning->zoom;
    re_form_world_coord(&panning->world_maze_pos);

    if (panning->currently_panning || d_screen_mouse_pixels != (vec2){0,0})
    {
      panning->currently_panning = true;
    }
  }
  else if (panning->currently_panning && mouse->l_on_up)
  {
    panning->currently_panning = false;
    mouse->l_on_up = false;
  }
}


b32
sim_tick(GameState *game_state, u64 time_us)
{
  b32 result = false;

  if (time_us >= game_state->last_sim_tick + (u32)(seconds_in_u(1) / game_state->sim_ticks_per_s))
  {
    if (game_state->single_step)
    {
      if (game_state->inputs.maps[STEP].active)
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
reset_zoom(GameState *game_state)
{
  game_state->world_per_pixel = 64*64;
  game_state->cell_margin = 0;

  Panning *panning = &game_state->panning;
  panning->last_mouse_pos = (vec2){0};
  panning->zoom = 0.1;
  panning->zoom_multiplier = 0;
  panning->scale_focus_pixels = (vec2){0, 0};

  panning->world_maze_pos.offset = (vec2){0, 0};

  // TODO: Centre maze at start
  vec2 maze_size = get_maze_size(&game_state->maze);
  vec2 maze_center = 0.5f * maze_size;
  panning->world_maze_pos.cell_x = maze_center.x;
  panning->world_maze_pos.cell_y = maze_center.y;
}


b32
load_maze(Memory *memory, GameState *game_state, u32 argc, char *argv[])
{
  b32 success = true;
  success &= parse(&game_state->maze, &game_state->functions, memory, game_state->filename);

  delete_all_cars(&game_state->cars);
  reset_car_inputs(&game_state->ui);

  game_state->finish_sim_step_move = false;
  game_state->last_sim_tick = 0;
  game_state->sim_steps = 0;

  return success;
}


b32
load_assets(Memory *memory, GameState *game_state, FT_Library *font_library)
{
  b32 success = true;

  if (load_font(font_library, &game_state->font_face))
  {
    success = false;
  }

  load_bitmap(&game_state->particles.spark_bitmap, "particles/spark.bmp");
  load_bitmap(&game_state->particles.cross_bitmap, "particles/cross.bmp");
  load_bitmap(&game_state->particles.blob_bitmap,  "particles/blob.bmp");
  load_bitmap(&game_state->particles.smoke_bitmap, "particles/smoke.bmp");
  load_bitmap(&game_state->bitmaps.tile, "tile.bmp");
  load_bitmap(&game_state->bitmaps.font, "font.bmp");

  load_cell_bitmaps(&game_state->cell_bitmaps);

  strcpy(game_state->persistent_str, "Init!");

  init_ui(&game_state->ui);

  XMLTag *arrow = load_xml("cells/arrow.svg", memory);
  // test_traverse_xml_struct(L_GameLoop, arrow);

  game_state->arrow_svg = 0;
  get_svg_operations(memory, arrow, &game_state->arrow_svg);

  return success;
}


b32
init_game(Memory *memory, GameState *game_state, Keys *keys, FT_Library *font_library, u64 time_us, u32 argc, char *argv[])
{
  b32 success = true;

  game_state->init = true;

  game_state->single_step = false;
  game_state->sim_ticks_per_s = 5;

  game_state->finish_sim_step_move = false;

  if (argc > 1)
  {
    game_state->filename = argv[1];
  }
  else
  {
    printf("Error: No Maze filename supplied.\n");
    success = false;
  }

  success &= load_maze(memory, game_state, argc, argv);
  reset_zoom(game_state);

  setup_inputs(keys, &game_state->inputs);

  success &= load_assets(memory, game_state, font_library);
  success &= setup_cell_instancing(&game_state->cell_instancing_vbos);

  add_all_cell_instances(&game_state->cell_instancing_vbos, &game_state->maze.tree);

  return success;
}


b32
update_and_render(Memory *memory, Renderer *renderer, FT_Library *font_library,
                  Keys *keys, Mouse *mouse, u64 time_us, u32 last_frame_dt, u32 fps,
                  u32 argc, char *argv[])
{
  b32 keep_running = true;
  vec2 screen_size = Vec2(renderer->width, renderer->height);

  static GameState *game_state = 0;
  if (game_state == 0)
  {
    game_state = push_struct(memory, GameState);

    if (!init_game(memory, game_state, keys, font_library, time_us, argc, argv))
    {
      keep_running = false;
      return keep_running;
    }
  }

  update_inputs(keys, &game_state->inputs, time_us);

  if (game_state->inputs.maps[SAVE].active)
  {
    serialize_maze(&game_state->maze, &game_state->functions, game_state->filename);
  }

  if (game_state->inputs.maps[RELOAD].active)
  {
    strcpy(game_state->persistent_str, "Reload!");
    load_maze(memory, game_state, argc, argv);
  }

  if (game_state->inputs.maps[RESET].active)
  {
    strcpy(game_state->persistent_str, "Reset!");
    reset_zoom(game_state);
  }

  if (game_state->inputs.maps[RESTART].active)
  {
    strcpy(game_state->persistent_str, "Restart!");
    delete_all_cars(&game_state->cars);
    reset_car_inputs(&game_state->ui);
    game_state->finish_sim_step_move = false;
    game_state->last_sim_tick = 0;
    game_state->sim_steps = 0;
  }

  if (game_state->inputs.maps[STEP_MODE_TOGGLE].active)
  {
    game_state->single_step = !game_state->single_step;
    log(L_GameLoop, "Changing stepping mode");
  }

  //
  // UPDATE VIEW
  //

  update_pan_and_zoom(&game_state->panning, &game_state->inputs, mouse, screen_size);


  //
  // UPDATE WORLD
  //

  // ui_consume_mouse_clicks(&game_state->ui, mouse, ui_mouse, time_us);

  if (game_state->inputs.maps[SIM_TICKS_INC].active)
  {
    game_state->sim_ticks_per_s += .5f;
  }
  if (game_state->inputs.maps[SIM_TICKS_DEC].active)
  {
    game_state->sim_ticks_per_s -= .5f;
  }
  game_state->sim_ticks_per_s = clamp(.5, game_state->sim_ticks_per_s, 20);

  // update_cells_ui_state(game_state, mouse, world_mouse, time_us);

  b32 sim = sim_tick(game_state, time_us);

  if (sim && game_state->ui.car_inputs == 0 && !game_state->finish_sim_step_move)
  {
    perform_cells_sim_tick(memory, game_state, &(game_state->maze.tree), time_us);
    perform_cars_sim_tick(memory, game_state, time_us);
  }

  if (sim && game_state->ui.car_inputs == 0)
  {
    move_cars(game_state);

    game_state->finish_sim_step_move = false;
    ++game_state->sim_steps;
  }

  annimate_cars(game_state, last_frame_dt);
  step_particles(&(game_state->particles), time_us);

  // update_ui(game_state, &game_state->ui, ui_mouse, &game_state->inputs, time_us);

  //
  // RENDER
  //

  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  r32 aspect = (r32)renderer->height / renderer->width;
  mat4 projection_matrix = {aspect, 0, 0, 0,
                            0, -1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1};

  draw_instanced_cells(&game_state->cell_instancing_vbos, &game_state->panning, projection_matrix);

  // draw_cells(game_state, &render_window, &(game_state->maze.tree), time_us);
  // draw_cars(game_state, &render_window, &(game_state->cars), time_us);
  // render_particles(&(game_state->particles), renderer, &render_basis);


  // draw_string(renderer, &orthographic_basis, &\game_state->bitmaps.font,
  //             size(game_state->screen_render_region) - CHAR_SIZE*text_scale*(vec2){strlen(game_state->persistent_str), 1},
  //             game_state->persistent_str, text_scale, (vec4){1, 0, 0, 0});

  // draw_ui(&render_window, &game_state->bitmaps.font, &game_state->cell_bitmaps, &game_state->ui, time_us);

  // char str[4];
  // fmted_str(str, 4, "%d", fps);
  // draw_string(renderer, &orthographic_basis, &game_state->bitmaps.font, (vec2){0, 0}, str, 0.3, (vec4){1, 0, 0, 0});

  keep_running &= print_gl_errors();
  // printf("Main loop end\n");
  return keep_running;
}