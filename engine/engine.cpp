u64
get_us()
{
  struct timeval tv;
  gettimeofday(&tv, 0);
  return ((u64)tv.tv_sec * (u64)1000000) + (u64)tv.tv_usec;
}


void
set_keys(Keys *keys)
{
  keys->updated = false;

  keys->up.on_up = false;
  keys->down.on_up = false;
  keys->left.on_up = false;
  keys->right.on_up = false;

  keys->up.on_down = false;
  keys->down.on_down = false;
  keys->left.on_down = false;
  keys->right.on_down = false;

  for (u32 i = 0; i < array_count(keys->alpha_num_sym); ++i)
  {
    keys->alpha_num_sym[i].on_up = false;
    keys->alpha_num_sym[i].on_down = false;
  }
}


void
set_mouse(Mouse *mouse)
{
  mouse->l_on_down = false;
  mouse->r_on_down = false;
  mouse->l_on_up = false;
  mouse->r_on_up = false;

  mouse->scroll.x = 0;
  mouse->scroll.y = 0;
}


b32
process_keys(Keys *keys, SDL_Event event)
{
  b32 quit = false;
  b32 pressed = (event.type == SDL_KEYDOWN);

  if ((pressed && event.key.keysym.sym == 'w' && event.key.keysym.mod == KMOD_LCTRL) ||
      (pressed && event.key.keysym.sym == SDLK_F4 && event.key.keysym.mod == KMOD_LALT))
  {
    quit = true;
  }
  else
  {
    Key *input = 0;
    SDL_Keysym key = event.key.keysym;
    if (key.sym >= MIN_CHAR && key.sym < MAX_CHAR)
    {
      input = keys->alpha_num_sym + key.sym - MIN_CHAR;
    }
    else
    {
      switch (key.sym)
      {
      case SDLK_UP:
        input = &keys->up;
        break;
      case SDLK_DOWN:
        input = &keys->down;
        break;
      case SDLK_LEFT:
        input = &keys->left;
        break;
      case SDLK_RIGHT:
        input = &keys->right;
        break;
      case SDLK_BACKSPACE:
        input = &keys->backspace;
        break;
      case SDLK_RETURN:
        input = &keys->enter;
        break;
      }
    }

    if (input)
    {
      input->on_up = !pressed && input->down;
      input->on_down = pressed && !input->down;

      keys->updated |= input->on_up || input->on_down;

      input->down = pressed;
    }
  }

  return quit;
}


void
process_mouse(Mouse *mouse, SDL_Event event)
{
  switch (event.type) {
    case SDL_MOUSEMOTION:
    {
      mouse->x = event.motion.x;
      mouse->y = event.motion.y;
    } break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    {
      b32 down = (event.type == SDL_MOUSEBUTTONDOWN);

      if (event.button.button == SDL_BUTTON_LEFT)
      {
        mouse->l_down = down;
        mouse->l_on_down = down;
        mouse->l_on_up = !down;
      }
      else if (event.button.button == SDL_BUTTON_RIGHT)
      {
        mouse->r_down = down;
        mouse->l_on_down = down;
        mouse->l_on_up = !down;
      }
    } break;

    case SDL_MOUSEWHEEL:
    {
      mouse->scroll.x = -event.wheel.x;
      mouse->scroll.y = -event.wheel.y;
    } break;
  }
}


void
game_loop(Memory *memory, Renderer *renderer, FT_Library *font_library, u32 argc, const u8 *argv[], UpdateAndRenderFunc update_and_render_func)
{
  b32 running = true;

  u32 useconds_per_frame = 1000000 / FPS;
  u32 frame_dt = useconds_per_frame;

  FPSCounter fps = {
    .frame_count = 0,
    .last_update = get_us()
  };

  Keys keys = {};
  Mouse mouse = {};
  mouse.x = -1;
  mouse.y = -1;

  while (running)
  {
    u64 last_frame_end = get_us();

    // mouse.scroll -= mouse.scroll / 6.0f;
    // r32 epsilon = 1.5f;
    // if (abs(mouse.scroll.y) < epsilon)
    // {
    //   mouse.scroll.y = 0;
    // }
    // if (abs(mouse.scroll.x) < epsilon)
    // {
    //   mouse.scroll.x = 0;
    // }

    set_keys(&keys);
    set_mouse(&mouse);

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
        {
          running = false;
        } break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
          running &= !process_keys(&keys, event);
        } break;

        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEWHEEL:
        {
          process_mouse(&mouse, event);
        } break;
      }
    }

    running &= update_and_render_func(memory, renderer, font_library, &keys, &mouse, last_frame_end, frame_dt, fps.current_avg, argc, argv);

    SDL_GL_SwapWindow(renderer->window);

    ++fps.frame_count;
    if (last_frame_end >= fps.last_update + seconds_in_u(1))
    {
      fps.last_update = last_frame_end;
      fps.current_avg = fps.frame_count;
      fps.frame_count = 0;
    }

    frame_dt = get_us() - last_frame_end;

    if (frame_dt < useconds_per_frame)
    {
      usleep(useconds_per_frame - frame_dt);
      frame_dt = useconds_per_frame;
    }
    else
    {
      log(L_Main, u8("Missed frame rate: %d"), frame_dt);
    }
  }
}


b32
start_engine(u32 argc, const u8 *argv[], UpdateAndRenderFunc update_and_render_func)
{
  b32 success = true;

  srand(time(0));

  Memory memory;
  memory.total = TOTAL_MEMORY;
  memory.memory = (u8 *)malloc(memory.total);
  memory.used = 0;

  Renderer renderer;

  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
      printf("Failed to init SDL.\n");
      success = false;
      return success;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_WindowFlags flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
  if (FULLSCREEN)
  {
    flags = (SDL_WindowFlags)(flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
  }
  else
  {
    renderer.width = WINDOW_WIDTH;
    renderer.height = WINDOW_HEIGHT;
  }

  renderer.window = SDL_CreateWindow("Maze Interpreter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, renderer.width, renderer.height, flags);
  if (!renderer.window)
  {
    printf("Failed to initialise SDL window: %s\n", SDL_GetError());
    success = false;
    return success;
  }

  if (FULLSCREEN)
  {
    s32 display_index = SDL_GetWindowDisplayIndex(renderer.window);
    if (display_index < 0)
    {
      printf("Failed to get display index.\n");
      success = false;
      return success;
    }
    SDL_Rect window_rect;
    if (SDL_GetDisplayBounds(display_index, &window_rect))
    {
      printf("Failed to get display bounds.\n");
      success = false;
      return success;
    }
    renderer.width = window_rect.w;
    renderer.height = window_rect.h;
  }

  renderer.gl_context = SDL_GL_CreateContext(renderer.window);
  if (!renderer.gl_context)
  {
    printf("Failed to create OpenGL context.\n");
    success = false;
    return success;
  }

  SDL_GL_SetSwapInterval(1);

  glewExperimental = GL_TRUE;
  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK)
  {
    printf("Failed to init GLEW: \"%s\"\n", glewGetErrorString(glew_status));
    success = false;
    return success;
  }

  glViewport(0, 0, renderer.width, renderer.height);
  glEnable(GL_DEPTH_TEST);

#ifdef DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(opengl_debug_output_callback, NULL);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif

  const unsigned char *opengl_version = glGetString(GL_VERSION);
  printf("OpenGL Version: %s\n", opengl_version);
  print_gl_errors();
  printf("OpenGL init finished.\n");

  FT_Library font_library;
  if (init_freetype(&font_library))
  {
    printf("Failed to init freetype library.\n");
    success = false;
    return success;
  }

  game_loop(&memory, &renderer, &font_library, argc, argv, update_and_render_func);

  SDL_GL_DeleteContext(renderer.gl_context);
  SDL_DestroyWindow(renderer.window);
  SDL_Quit();
  free(memory.memory);

  return success;
}