b32
compile_shader(const char shader_source[], s32 size, GLenum shader_type, GLuint *shader)
{
  b32 success = true;

  *shader = glCreateShader(shader_type);

  glShaderSource(*shader, 1, &shader_source, &size);
  glCompileShader(*shader);

  GLint compile_success = 0;
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &compile_success);
  if (!compile_success)
  {
    GLint log_size = 0;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &log_size);

    GLchar *info_log = (GLchar *)malloc(sizeof(GLchar) * log_size);
    glGetShaderInfoLog(*shader, log_size, NULL, info_log);

    log(L_OpenGL, "Shader compile error:\n%s", info_log);
    free(info_log);

    glDeleteShader(*shader);
    *shader = 0;
    success = false;
  }

  return success;
}


b32
load_shader(const char filename[], GLenum shader_type, GLuint *shader)
{
  b32 success = true;
  *shader = 0;

  File file;
  if (open_file(filename, &file))
  {
    success = compile_shader(file.text, file.size, shader_type, shader);
  }
  else
  {
    success = false;
  }

  return success;
}


b32
create_shader_program(const char *filenames[], GLenum types[], s32 size, GLuint *shader_program)
{
  b32 success = true;
  *shader_program = glCreateProgram();

  for (u32 i = 0; i < size; ++i)
  {
    GLuint shader;
    b32 shader_success = load_shader(filenames[i], types[i], &shader);
    if (shader_success == 0)
    {
      success = false;
      break;
    }
    glAttachShader(*shader_program, shader);
  }

  if (success)
  {
    glLinkProgram(*shader_program);

    GLint is_linked = 0;
    glGetProgramiv(*shader_program, GL_LINK_STATUS, &is_linked);
    if (is_linked == GL_FALSE)
    {
      GLint log_size = 0;
      glGetProgramiv(*shader_program, GL_INFO_LOG_LENGTH, &log_size);

      GLchar *info_log = (GLchar *)malloc(sizeof(GLchar) * log_size);
      glGetProgramInfoLog(*shader_program, log_size, NULL, info_log);

      log(L_OpenGL, "Shader link error:\n%s", info_log);
      free(info_log);

      glDeleteProgram(*shader_program);

      success = false;
    }
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


b32
print_gl_errors()
{
  b32 success = true;

  GLenum error = glGetError();
  while (error != GL_NO_ERROR)
  {
    success &= false;
    gl_print_error(error);

    error = glGetError();
  }

  return success;
}


GLuint
create_buffer()
{
  GLuint result;
  glGenBuffers(1, &result);
  return result;
}


GLuint
setup_vao()
{
  GLuint result;
  glGenVertexArrays(1, &result);
  glBindVertexArray(result);
  return result;
}


b32
get_uniform_locations(GLuint shader_program, Uniform *uniforms, u32 n_uniforms)
{
  b32 success = true;

  for (u32 i = 0;
       i < n_uniforms;
       ++i)
  {
    Uniform *current = uniforms + i;

    current->location = glGetUniformLocation(shader_program, current->name);
    if (current->location == -1)
    {
      log(L_OpenGL, "Failed to find uniform %s", current->name);
      current->location = 0;
      success &= false;
    }
  }

  return success;
}
