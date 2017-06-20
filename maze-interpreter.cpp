void
update_pan_and_zoom(GameState *game_state, Mouse *mouse, V2 screen_size)
{
  // Convert to 0-centred coords from 0-top-left
  V2 screen_mouse_pixels = (V2){mouse->x, mouse->y} - (0.5 * screen_size);

  r32 old_zoom_multiplier = game_state->zoom_multiplier;
  game_state->zoom_multiplier -= mouse->scroll.y * 0.01;

  if (game_state->inputs.maps[ZOOM_IN].active)
  {
    game_state->zoom_multiplier += .2;
  }
  if (game_state->inputs.maps[ZOOM_OUT].active)
  {
    game_state->zoom_multiplier -= .2;
  }

  // If just started zooming: use this mouse position for scale focus
  // if (old_zoom_multiplier < 0.0005 && game_state->zoom_multiplier > 0.001)
  {
    game_state->scale_focus_pixels = screen_mouse_pixels;
  }

  game_state->old_zoom = game_state->zoom;
  game_state->zoom *= 1 + game_state->zoom_multiplier;
  game_state->zoom = max(0.005, min(1.0, game_state->zoom));
  game_state->zoom_multiplier *= 0.9f;

  V2 d_screen_mouse_pixels = screen_mouse_pixels - game_state->last_mouse_pos;
  game_state->last_mouse_pos = screen_mouse_pixels;

  // TODO: Need a layer to filter whether the mouse is focused on the world
  if (mouse->l_down)
  {
    game_state->world_maze_pos.offset -= d_screen_mouse_pixels / (game_state->zoom * 270);
    re_form_world_coord(&game_state->world_maze_pos);

    if (game_state->currently_panning || d_screen_mouse_pixels != (V2){0,0})
    {
      game_state->currently_panning = true;
    }
  }
  else if (game_state->currently_panning && mouse->l_on_up)
  {
    game_state->currently_panning = false;
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
  // TODO: Should world coords be r32s now we are using u32s for
  //       the cell position?
  game_state->world_per_pixel = 64*64;
  game_state->cell_margin = 0;
  game_state->last_mouse_pos = (V2){0};

  game_state->zoom = 0.1;
  game_state->zoom_multiplier = 0;
  game_state->scale_focus_pixels = (V2){0, 0};

  game_state->world_maze_pos.offset = (V2){0, 0};

  // TODO: Centre maze at start
  V2 maze_size = get_maze_size(&game_state->maze);
  V2 maze_center = 0.5f * maze_size;
  game_state->world_maze_pos.cell_x = maze_center.x;
  game_state->world_maze_pos.cell_y = maze_center.y;
}


bool
load_maze(Memory *memory, GameState *game_state, u32 argc, char *argv[])
{
  bool success = true;
  success &= parse(&game_state->maze, &game_state->functions, memory, game_state->filename);

  delete_all_cars(&game_state->cars);
  reset_car_inputs(&game_state->ui);

  game_state->finish_sim_step_move = false;
  game_state->last_sim_tick = 0;
  game_state->sim_steps = 0;

  return success;
}


bool
load_assets(Memory *memory, GameState *game_state, FT_Library *font_library)
{
  bool success = true;

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


bool
init_game(Memory *memory, GameState *game_state, Keys *keys, FT_Library *font_library, u64 time_us, u32 argc, char *argv[])
{
  bool success = true;

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
  success &= load_shaders(&game_state->shader_program);

  success &= initialise_vbo(&game_state->shader_program, &game_state->vbo_id);

  return success;
}


bool
update_and_render(Memory *memory, GameState *game_state, Renderer *renderer, FT_Library *font_library,
                  Keys *keys, Mouse *mouse, u64 time_us, u32 last_frame_dt, u32 fps,
                  u32 argc, char *argv[])
{
  bool keep_running = true;
  V2 screen_size = (V2){renderer->width, renderer->height};

  if (!game_state->init)
  {
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

  update_pan_and_zoom(game_state, mouse, screen_size);

// TODO: Get zooming at mouse pointer working
#if 0
    // Convert from -0.5*size -> 0.5*size coordinates, to size -> 0
    V2 scale_focus_gl_pixels = game_state->scale_focus_pixels + screen_size * 0.5;
    scale_focus_gl_pixels.y = screen_size.y - scale_focus_gl_pixels.y;

    // Get the world coordinates of the pixel scale focus
    GLdouble scale_focus_object_z;
    glReadPixels(scale_focus_gl_pixels.x, scale_focus_gl_pixels.y, 1 , 1, GL_DEPTH_COMPONENT, GL_DOUBLE, &scale_focus_object_z);

    GLdouble scale_focus_world_x,
             scale_focus_world_y,
             scale_focus_world_z;
    gluUnProject(scale_focus_gl_pixels.x, scale_focus_gl_pixels.y, scale_focus_object_z,
                 game_state->world_transform.modelview,
                 game_state->world_transform.projection,
                 game_state->viewport,
                 &scale_focus_world_x, &scale_focus_world_y, &scale_focus_world_z);
    V2 scale_focus_world = (V2){scale_focus_world_x, scale_focus_world_y};


    // glScalef(game_state->zoom, game_state->zoom, 1);


    game_state->world_maze_pos.offset += scale_focus_world/game_state->old_zoom - scale_focus_world/game_state->zoom;
    re_form_world_coord(&game_state->world_maze_pos);

    log(L_GameLoop, "Zoom: %f", game_state->zoom);
    log(L_GameLoop, "Scale Focus: (%f, %f)", scale_focus_world.x, scale_focus_world.y);
    log(L_GameLoop, "World Pos: (%d, %d) . (%f, %f)", game_state->world_maze_pos.cell_x, game_state->world_maze_pos.cell_y,
                                                      game_state->world_maze_pos.offset.x, game_state->world_maze_pos.offset.y);

    glPushMatrix();
      glTranslatef(scale_focus_world.x, scale_focus_world.y, 0);
      glColor3f(0, 0, 0);
      draw_box();
    glPopMatrix();
#endif

  //   glScalef(game_state->zoom, game_state->zoom, 1);

  //   // Save the world's matrices
  //   glGetDoublev( GL_MODELVIEW_MATRIX, game_state->world_transform.modelview);
  //   glGetDoublev(GL_PROJECTION_MATRIX, game_state->world_transform.projection);

  // glPopMatrix();

  // Screen space
  // glPushMatrix();
  //   glMatrixMode(GL_MODELVIEW);
  //   glLoadIdentity();
  //   glOrtho(-0.5*renderer->width,   0.5*renderer->width,
  //            0.5*renderer->height, -0.5*renderer->height, 1, -1);

  //   glGetDoublev( GL_MODELVIEW_MATRIX, game_state->ui_transform.modelview);
  //   glGetDoublev(GL_PROJECTION_MATRIX, game_state->ui_transform.projection);
  // glPopMatrix();


  RenderWindow render_window = {game_state->world_maze_pos, (Rectangle){}};

  // TODO: This line causing a GL_INVALID_ENUM
  // V2 render_window_mouse = un_project_mouse(game_state->viewport, &game_state->world_transform, renderer, mouse);
  // WorldSpace world_mouse = render_window_coord_to_world_coord(&render_window, render_window_mouse);

  // V2 ui_mouse = un_project_mouse(game_state->viewport, &game_state->ui_transform, renderer, mouse);

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

  // World Space
  // load_matrix(&game_state->world_transform);

  // draw_cells(game_state, &render_window, &(game_state->maze.tree), time_us);
  // draw_cars(game_state, &render_window, &(game_state->cars), time_us);
  // render_particles(&(game_state->particles), renderer, &render_basis);
  // glTranslatef(-(r32)game_state->world_maze_pos.cell_x-game_state->world_maze_pos.offset.x,
  //              -(r32)game_state->world_maze_pos.cell_y-game_state->world_maze_pos.offset.y, 0);
  // draw_svg(game_state->arrow_svg);

  // Screen Space
  // load_matrix(&game_state->ui_transform);
  // glPushMatrix();
  //   glScalef(0.2, 0.2, 1);
  //   draw_string(font_library, game_state->font_face, String("Hello World!"));
  // glPopMatrix();

  // r32 text_scale = 0.3;
  // draw_string(renderer, &orthographic_basis, &\game_state->bitmaps.font,
  //             size(game_state->screen_render_region) - CHAR_SIZE*text_scale*(V2){strlen(game_state->persistent_str), 1},
  //             game_state->persistent_str, text_scale, (V4){1, 0, 0, 0});

  // draw_ui(&render_window, &game_state->bitmaps.font, &game_state->cell_bitmaps, &game_state->ui, time_us);

  // char str[4];
  // fmted_str(str, 4, "%d", fps);
  // draw_string(renderer, &orthographic_basis, &game_state->bitmaps.font, (V2){0, 0}, str, 0.3, (V4){1, 0, 0, 0});


  // Shader rendering

  if (game_state->vbo_size == 0)
  {
    ShaderAttributes coords[] = {
      {.world_cell_position_x =  0, .world_cell_position_y =  0, .world_cell_offset = {0, 0}, .colour = {1, 0, 0, 1}},
      {.world_cell_position_x =  0, .world_cell_position_y =  0, .world_cell_offset = {1, 0}, .colour = {1, 0, 0, 1}},
      {.world_cell_position_x =  0, .world_cell_position_y =  0, .world_cell_offset = {1, 1}, .colour = {1, 0, 0, 1}},
      {.world_cell_position_x =  0, .world_cell_position_y =  0, .world_cell_offset = {0, 1}, .colour = {1, 0, 0, 1}},
    };
    game_state->vbo_size = array_count(coords);
    load_coords_into_vbo(game_state->vbo_id, coords, game_state->vbo_size);
  }

  glUniform1i(game_state->shader_program.uniform_int_render_origin_cell_x, 0);
  glUniform1i(game_state->shader_program.uniform_int_render_origin_cell_y, 0);
  glUniform2f(game_state->shader_program.uniform_vec2_render_origin_offset, 0.5, 0);
  glUniform1f(game_state->shader_program.uniform_float_scale, 0.5);

  glUseProgram(game_state->shader_program.program);
  glEnableVertexAttribArray(game_state->shader_program.attribute_world_cell_position_x);
  glEnableVertexAttribArray(game_state->shader_program.attribute_world_cell_position_y);
  glEnableVertexAttribArray(game_state->shader_program.attribute_world_cell_offset);
  glEnableVertexAttribArray(game_state->shader_program.attribute_colour);

  glDrawArrays(GL_QUADS, 0, game_state->vbo_size);

  glDisableVertexAttribArray(game_state->shader_program.attribute_world_cell_position_x);
  glDisableVertexAttribArray(game_state->shader_program.attribute_world_cell_position_y);
  glDisableVertexAttribArray(game_state->shader_program.attribute_world_cell_offset);
  glDisableVertexAttribArray(game_state->shader_program.attribute_colour);

  gl_get_errors();

  return keep_running;
}