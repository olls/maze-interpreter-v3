void
update_pan_and_zoom(GameState *game_state, Mouse *mouse)
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

  V2 screen_mouse_pixels = (V2){mouse->x, mouse->y};
  V2 d_screen_mouse_pixels = screen_mouse_pixels - game_state->last_mouse_pos;

  game_state->last_mouse_pos = screen_mouse_pixels;

  // TODO: Need a layer to filter whether the mouse is focused on the world
  if (mouse->l_down)
  {
    game_state->maze_pos += d_screen_mouse_pixels;

    if (game_state->currently_panning || d_screen_mouse_pixels != (V2){0,0})
    {
      game_state->currently_panning = true;
    }
  }
  else
  {
    game_state->currently_panning = false;
  }

  if (abs(game_state->zoom - old_zoom) > 0.1)
  {
    game_state->scale_focus = untransform_coord(&game_state->last_render_basis, screen_mouse_pixels);
  }
}


void *
consume_from_render_queue(void *q)
{
  RenderQueue *render_queue = (RenderQueue *)q;

  while (true)
  {
    pthread_mutex_lock(&render_queue->mut);

    while (render_queue->empty)
    {
      log(L_RenderQueue, "consumer: queue EMPTY.");
      pthread_cond_wait(&render_queue->not_empty, &render_queue->mut);
    }

    RenderQueueData render_data;
    queue_del(render_queue, &render_data);

    pthread_mutex_unlock(&render_queue->mut);
    pthread_cond_signal(&render_queue->not_full);

    consume_render_operations(render_data.frame_buffer, render_data.render_operations, render_data.clip_region);
    log(L_RenderQueue, "consumer: recieved.");
  }

  return 0;
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
reset_zoom(GameState *game_state)
{
  // TODO: Should world coords be r32s now we are using u32s for
  //       the cell position?
  game_state->world_per_pixel = 64*64;
  game_state->cell_spacing = 100000;
  game_state->cell_margin = 0.2f;
  game_state->last_mouse_pos = (V2){0};

  // NOTE: Somewhere between the sqrt( [ MIN, MAX ]_WORLD_PER_PIXEL )
  game_state->zoom = 30;

  V2 maze_size_in_pixels = (V2){game_state->maze.width, game_state->maze.height} * game_state->cell_spacing / game_state->world_per_pixel;
  game_state->maze_pos = (0.5f * size(game_state->world_render_region)) - (0.5f * maze_size_in_pixels);
}


void
load_maze(Memory *memory, GameState *game_state, u32 argc, char *argv[])
{
  parse(&game_state->maze, memory, game_state->filename);

  V2 size = get_maze_size(&game_state->maze);
  game_state->maze.width = size.x;
  game_state->maze.height = size.y;

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

  if (argc > 1)
  {
    game_state->filename = argv[1];
  }
  else
  {
    printf("Error: No Maze filename supplied.\n");
    exit(1);
  }

  setup_inputs(keys, game_state->inputs);

  load_bitmap(&game_state->particles.spark_bitmap, "particles/spark.bmp");
  load_bitmap(&game_state->particles.cross_bitmap, "particles/cross.bmp");
  load_bitmap(&game_state->particles.blob_bitmap, "particles/blob.bmp");
  load_bitmap(&game_state->particles.smoke_bitmap, "particles/smoke.bmp");
  load_bitmap(&game_state->bitmaps.tile, "tile.bmp");
  load_bitmap(&game_state->bitmaps.font, "font.bmp");

  strcpy(game_state->persistent_str, "Init!");

  init_ui(&game_state->ui);
}


void
init_render_segments(Memory *memory, GameState *game_state, FrameBuffer *frame_buffer)
{
  game_state->screen_render_region.start = (V2){0, 0};
  game_state->screen_render_region.end = (V2){frame_buffer->width, frame_buffer->height};
  game_state->screen_render_region = grow(game_state->screen_render_region, -20);

  game_state->world_render_region = grow(game_state->screen_render_region, -10);

  V2 ns = {4, 4};
  game_state->render_segs.segments = push_structs(memory, Rectangle, ns.x * ns.y);
  game_state->render_segs.n_segments = ns;

  V2 s;
  for (s.y = 0; s.y < ns.y; ++s.y)
  {
    for (s.x = 0; s.x < ns.x; ++s.x)
    {
      Rectangle *segment = game_state->render_segs.segments + (u32)s.y + (u32)ns.y*(u32)s.x;
      *segment = grow(get_segment(game_state->screen_render_region, s, ns), 0);
    }
  }

  game_state->render_queue.empty = true;
  game_state->render_queue.full = false;
  game_state->render_queue.head = 0;
  game_state->render_queue.tail = 0;

  pthread_mutex_init(&game_state->render_queue.mut, 0);
  pthread_cond_init(&game_state->render_queue.not_full, 0);
  pthread_cond_init(&game_state->render_queue.not_empty, 0);

  for (u32 t = 0;
       t < ns.x*ns.y;
       ++t)
  {
    pthread_t thread;
    pthread_create(&thread, 0, consume_from_render_queue, &game_state->render_queue);
  }
}


void
update_and_render(Memory *memory, GameState *game_state, FrameBuffer *frame_buffer, Keys *keys, Mouse *mouse, u64 time_us, u32 last_frame_dt, u32 fps , u32 argc, char *argv[])
{

  if (!game_state->init)
  {
    init_render_segments(memory, game_state, frame_buffer);
    init_game(memory, game_state, keys, time_us, argc, argv);
    load_maze(memory, game_state, argc, argv);
    reset_zoom(game_state);
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
    reset_zoom(game_state);
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

  //
  // UPDATE VIEW
  //

  update_pan_and_zoom(game_state, mouse);

  RenderBasis render_basis;
  render_basis.world_per_pixel = game_state->world_per_pixel;
  render_basis.scale = squared(game_state->zoom / 30.0f);
  render_basis.scale_focus = game_state->scale_focus;
  render_basis.origin = game_state->maze_pos * game_state->world_per_pixel;
  render_basis.clip_region = game_state->world_render_region * game_state->world_per_pixel;

  RenderBasis orthographic_basis;
  get_orthographic_basis(&orthographic_basis, game_state->screen_render_region);

  //
  // UPDATE WORLD
  //

  if (game_state->inputs[SIM_TICKS_INC].active)
  {
    game_state->sim_ticks_per_s += .5f;
  }
  if (game_state->inputs[SIM_TICKS_DEC].active)
  {
    game_state->sim_ticks_per_s -= .5f;
  }
  game_state->sim_ticks_per_s = clamp(.5, game_state->sim_ticks_per_s, 20);

  update_cells_ui_state(game_state, &render_basis, mouse, time_us);

  if (sim_tick(game_state, time_us))
  {
    perform_cells_sim_tick(memory, game_state, &(game_state->maze.tree), time_us);
    perform_cars_sim_tick(memory, game_state, time_us);

    ++game_state->sim_steps;
  }

  annimate_cars(game_state, last_frame_dt);
  step_particles(&(game_state->particles), time_us);

  update_ui(game_state, &orthographic_basis, &game_state->ui, mouse, time_us);

  //
  // RENDER
  //

  // Add render operations to queue

  game_state->render_operations.next_free = 0;

  add_fast_box_to_render_list(&game_state->render_operations, &orthographic_basis, (Rectangle){(V2){0,0},size(game_state->screen_render_region)}, (PixelColor){255, 255, 255});

  draw_cells(game_state, &game_state->render_operations, &render_basis, &(game_state->maze.tree), time_us);
  draw_cars(game_state, &game_state->render_operations, &render_basis, &(game_state->cars), time_us);
  // render_particles(&(game_state->particles), &game_state->render_operations, &render_basis);

  draw_string(&game_state->render_operations, &render_basis, &game_state->bitmaps.font, (V2){0, game_state->maze.height} * game_state->cell_spacing, game_state->persistent_str, 0.3, (V4){1, 0, 0, 0});

  draw_ui(&game_state->render_operations, &orthographic_basis, &game_state->bitmaps, &game_state->ui, time_us);

  char str[256];
  fmted_str(str, "%d", fps);
  draw_string(&game_state->render_operations, &orthographic_basis, &game_state->bitmaps.font, (V2){0, 0}, str, 0.3, (V4){1, 0, 0, 0});

  // Add render segments to render queue

  V2 ns = game_state->render_segs.n_segments;
  V2 s;
  for (s.y = 0; s.y < ns.y; ++s.y)
  {
    for (s.x = 0; s.x < ns.x; ++s.x)
    {
      RenderQueueData render_data;

      Rectangle *segment = game_state->render_segs.segments + (u32)s.y + (u32)ns.y*(u32)s.x;
      render_data.clip_region = *segment;

      render_data.frame_buffer = frame_buffer;
      render_data.render_operations = &game_state->render_operations;

      pthread_mutex_lock(&game_state->render_queue.mut);

      while (game_state->render_queue.full)
      {
        log(L_RenderQueue, "producer: queue FULL.");
        pthread_cond_wait(&game_state->render_queue.not_full, &game_state->render_queue.mut);
      }

      log(L_RenderQueue, "producer: Adding.");
      queue_add(&game_state->render_queue, render_data);

      pthread_mutex_unlock(&game_state->render_queue.mut);
      pthread_cond_signal(&game_state->render_queue.not_empty);
    }
  }

  // TODO: Wait for frame rendering to finish

  // TODO: Get rid of this
  game_state->last_render_basis = render_basis;
  game_state->panning_this_frame = game_state->currently_panning;
}