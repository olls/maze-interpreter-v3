void
load_debug_persistent_str(const u8 *string, GameState *game_state)
{
  copy_string(game_state->persistent_str, string, array_count(game_state->persistent_str));
}


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
load_maze(Memory *memory, GameState *game_state)
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
load_assets(Memory *memory, GameState *game_state)
{
  b32 success = true;

  load_bitmap(&game_state->particles.spark_bitmap, u8("particles/spark.bmp"));
  load_bitmap(&game_state->particles.cross_bitmap, u8("particles/cross.bmp"));
  load_bitmap(&game_state->particles.blob_bitmap,  u8("particles/blob.bmp"));
  load_bitmap(&game_state->particles.smoke_bitmap, u8("particles/smoke.bmp"));

  load_cell_bitmaps(&game_state->cell_bitmaps);

  load_debug_persistent_str(u8("Init!"), game_state);

  init_ui(&game_state->ui);

  XMLTag *arrow = load_xml(u8("cells/arrow.svg"), memory);
  // test_traverse_xml_struct(L_GameLoop, arrow);

  game_state->arrow_svg = 0;
  get_svg_operations(memory, arrow, &game_state->arrow_svg);

  success &= parse_otf_file(u8("fonts/DejaVuSansMono.ttf"), &game_state->font);

  return success;
}


b32
init_opengl(GameState *game_state)
{
  b32 success = true;

  success &= setup_cell_instancing(&game_state->cell_instancing);
  success &= setup_screen_space_rendering(&game_state->screen_space_rendering);

  setup_general_vertices(&game_state->general_vertices);
  game_state->general_screen_vao = setup_general_vertices_to_screen_space_vao(&game_state->general_vertices);

  return success;
}


b32
init_game(Memory *memory, GameState *game_state, Keys *keys, u64 time_us, u32 argc, const u8 *argv[])
{
  b32 success = true;

  game_state->init = true;

  game_state->single_step = false;
  game_state->sim_ticks_per_s = 5;

  game_state->finish_sim_step_move = false;

  b32 have_filename = false;
  if (argc > 1)
  {
    game_state->filename = argv[1];
    have_filename = true;
  }
  else
  {
    printf("Error: No Maze filename supplied.\n");
    have_filename = false;
  }

  if (!have_filename)
  {
    success = false;
  }
  else
  {
    b32 assets_loaded = load_assets(memory, game_state);
    b32 maze_loaded = load_maze(memory, game_state);
    b32 opengl_inited = init_opengl(game_state);

    if (assets_loaded &&
        maze_loaded &&
        opengl_inited)
    {
      setup_inputs(keys, &game_state->inputs);
      reset_zoom(game_state);

      add_all_cell_instances(&game_state->cell_instancing, &game_state->maze.tree);

      add_glyph_to_general_vertices(&game_state->font, &game_state->general_vertices, memory, U'{',
                                    &game_state->test_character_vbo, &game_state->test_character_ibo);
    }
    else
    {
      success = false;
    }
  }

  return success;
}


b32
update_and_render(Memory *memory, Memory *frame_memory, Renderer *renderer,
                  Keys *keys, Mouse *mouse, u64 time_us, u32 last_frame_dt, u32 fps,
                  u32 argc, const u8 *argv[])
{
  b32 keep_running = true;
  vec2 screen_size = Vec2(renderer->width, renderer->height);

  static GameState *game_state = 0;
  if (game_state == 0)
  {
    game_state = push_struct(memory, GameState);

    if (!init_game(memory, game_state, keys, time_us, argc, argv))
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
    load_debug_persistent_str(u8("Reload!"), game_state);
    load_maze(memory, game_state);
  }

  if (game_state->inputs.maps[RESET].active)
  {
    load_debug_persistent_str(u8("Reset!"), game_state);
    reset_zoom(game_state);
  }

  if (game_state->inputs.maps[RESTART].active)
  {
    load_debug_persistent_str(u8("Restart!"), game_state);
    delete_all_cars(&game_state->cars);
    reset_car_inputs(&game_state->ui);
    game_state->finish_sim_step_move = false;
    game_state->last_sim_tick = 0;
    game_state->sim_steps = 0;
  }

  if (game_state->inputs.maps[STEP_MODE_TOGGLE].active)
  {
    game_state->single_step = !game_state->single_step;
    log(L_GameLoop, u8("Changing stepping mode"));
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
                                0,  0, 1, 0,
                                0,  0, 0, 1};

  draw_instanced_cells(&game_state->cell_instancing, &game_state->panning, projection_matrix);

  debug_render_font_outline(game_state->general_screen_vao, &game_state->screen_space_rendering, &game_state->general_vertices, game_state->test_character_vbo, game_state->test_character_ibo);

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
  log(L_GameLoop, u8("Main loop end"));
  return keep_running;
}