void
print_gl_shader_log(GLenum shader)
{
  if (glIsShader(shader))
  {
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    char *program_log = (char *)malloc(length);
    glGetShaderInfoLog(shader, length, NULL, program_log);

    log(L_OpenGL, "Error: GLShader compile error: %.*s", length, program_log);
    free(program_log);
  }
  else
  {
    log(L_OpenGL, "Error: Argument passed to print_gl_shader_log is not a shader");
  }
}


void
print_gl_program_log(GLenum program)
{
  if (glIsProgram(program))
  {
    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    char *program_log = (char *)malloc(length);
    glGetProgramInfoLog(program, length, NULL, program_log);

    log(L_OpenGL, "Error: GL Program link error: %.*s", length, program_log);
    free(program_log);
  }
  else
  {
    log(L_OpenGL, "Error: Argument passed to print_gl_program_log is not a program");
  }
}


bool
compile_shader(const char filename[], GLenum type, GLuint *result)
{
  bool success = true;

  *result = glCreateShader(type);

  File source;
  success &= open_file(filename, &source);

  if (success)
  {
    glShaderSource(*result, 1, &source.read_only, NULL);
    glCompileShader(*result);

    close_file(&source);

    GLint compile_success = GL_FALSE;
    glGetShaderiv(*result, GL_COMPILE_STATUS, &compile_success);
    if (!compile_success)
    {
      print_gl_shader_log(*result);
      glDeleteShader(*result);
      success = false;
    }
  }

  if (!success)
  {
    *result = 0;
  }

  return success;
}


void
gl_print_error(GLenum error_code)
{
  switch (error_code)
  {
    case (GL_INVALID_ENUM):
    {
      log(L_OpenGL, "OpenGL error: GL_INVALID_ENUM");
    } break;
    case (GL_INVALID_VALUE):
    {
      log(L_OpenGL, "OpenGL error: GL_INVALID_VALUE");
    } break;
    case (GL_INVALID_OPERATION):
    {
      log(L_OpenGL, "OpenGL error: GL_INVALID_OPERATION");
    } break;
    case (GL_STACK_OVERFLOW):
    {
      log(L_OpenGL, "OpenGL error: GL_STACK_OVERFLOW");
    } break;
    case (GL_STACK_UNDERFLOW):
    {
      log(L_OpenGL, "OpenGL error: GL_STACK_UNDERFLOW");
    } break;
    case (GL_OUT_OF_MEMORY):
    {
      log(L_OpenGL, "OpenGL error: GL_OUT_OF_MEMORY");
    } break;
    case (GL_INVALID_FRAMEBUFFER_OPERATION):
    {
      log(L_OpenGL, "OpenGL error: GL_INVALID_FRAMEBUFFER_OPERATION");
    } break;
    case (GL_TABLE_TOO_LARGE):
    {
      log(L_OpenGL, "OpenGL error: GL_TABLE_TOO_LARGE");
    }
  }
}


bool
gl_get_errors()
{
  bool success = true;

  GLenum error = glGetError();
  while (error != GL_NO_ERROR)
  {
    success &= false;
    gl_print_error(error);

    error = glGetError();
  }

  return success;
}


bool
bind_attributes(OpenGLShaderProgram *open_gl_shader_program, GLuint shader_program)
{
  open_gl_shader_program->attribute_world_cell_position_x = 0;
  open_gl_shader_program->attribute_world_cell_position_y = 1;
  open_gl_shader_program->attribute_world_cell_offset = 2;
  open_gl_shader_program->attribute_colour = 4;

  glBindAttribLocation(shader_program, open_gl_shader_program->attribute_world_cell_position_x, "world_cell_position_x");
  glBindAttribLocation(shader_program, open_gl_shader_program->attribute_world_cell_position_y, "world_cell_position_y");
  glBindAttribLocation(shader_program, open_gl_shader_program->attribute_world_cell_offset, "world_cell_offset");
  glBindAttribLocation(shader_program, open_gl_shader_program->attribute_colour, "colour");

  glBindFragDataLocation(shader_program, 0, "colour_out");

  bool success = gl_get_errors();
  return success;
}


bool
get_uniform_locations(OpenGLShaderProgram *open_gl_shader_program)
{
  bool success = true;

  open_gl_shader_program->uniform_int_render_origin_cell_x = glGetUniformLocation(open_gl_shader_program->program, "render_origin_cell_x");
  open_gl_shader_program->uniform_int_render_origin_cell_y = glGetUniformLocation(open_gl_shader_program->program, "render_origin_cell_y");
  open_gl_shader_program->uniform_vec2_render_origin_offset = glGetUniformLocation(open_gl_shader_program->program, "render_origin_offset");
  open_gl_shader_program->uniform_float_scale = glGetUniformLocation(open_gl_shader_program->program, "scale");

  if (open_gl_shader_program->uniform_int_render_origin_cell_x == -1)
  {
    log(L_OpenGL, "Failed to find uniform int render_origin_cell_x");
    success &= false;
  }
  if (open_gl_shader_program->uniform_int_render_origin_cell_y == -1)
  {
    log(L_OpenGL, "Failed to find uniform int render_origin_cell_y");
    success &= false;
  }
  if (open_gl_shader_program->uniform_vec2_render_origin_offset == -1)
  {
    log(L_OpenGL, "Failed to find uniform vec2 render_origin_offset");
    success &= false;
  }
  if (open_gl_shader_program->uniform_float_scale == -1)
  {
    log(L_OpenGL, "Failed to find uniform float scale");
    success &= false;
  }

  return success;
}


bool
load_shaders(OpenGLShaderProgram *open_gl_shader_program)
{
  bool success = true;

  bool compile_success = true;
  GLuint vertex_shader = 0;
  compile_success &= compile_shader("vertex-shader.glvs", GL_VERTEX_SHADER, &vertex_shader);
  GLuint fragment_shader = 0;
  compile_success &= compile_shader("fragment-shader.glfs", GL_FRAGMENT_SHADER, &fragment_shader);

  if (compile_success)
  {
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    bool bind_success = bind_attributes(open_gl_shader_program, shader_program);
    if (!bind_success)
    {
      log(L_OpenGL, "Error: Could not bind attributes.");
      success &= false;
    }
    else
    {
      glLinkProgram(shader_program);

      GLint link_success = GL_FALSE;
      glGetProgramiv(shader_program, GL_LINK_STATUS, &link_success);
      if (!link_success)
      {
        log(L_OpenGL, "Error: Could not link shader program.");
        success &= false;
      }
    }

    if (success)
    {
      open_gl_shader_program->program = shader_program;
    }
    else
    {
      log(L_OpenGL, "Failed at shader linking");
      glDeleteProgram(shader_program);
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader);
    }
  }
  else
  {
    log(L_OpenGL, "Failed at shader compilation");
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
  }

  if (success)
  {
    bool uniforms_success = get_uniform_locations(open_gl_shader_program);
    if (!uniforms_success)
    {
      log(L_OpenGL, "Error: Could not get unioform locations.");
      success &= false;
    }
  }

  return success;
}
