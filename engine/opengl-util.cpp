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
gl_print_error(GLenum error_code, const char *file, u32 line)
{
  const char *error;
  switch (error_code)
  {
    case (GL_INVALID_ENUM):
    {
      error = "GL_INVALID_ENUM";
    } break;
    case (GL_INVALID_VALUE):
    {
      error = "GL_INVALID_VALUE";
    } break;
    case (GL_INVALID_OPERATION):
    {
      error = "GL_INVALID_OPERATION";
    } break;
    case (GL_STACK_OVERFLOW):
    {
      error = "GL_STACK_OVERFLOW";
    } break;
    case (GL_STACK_UNDERFLOW):
    {
      error = "GL_STACK_UNDERFLOW";
    } break;
    case (GL_OUT_OF_MEMORY):
    {
      error = "GL_OUT_OF_MEMORY";
    } break;
    case (GL_INVALID_FRAMEBUFFER_OPERATION):
    {
      error = "GL_INVALID_FRAMEBUFFER_OPERATION";
    } break;
    case (GL_TABLE_TOO_LARGE):
    {
      error = "GL_TABLE_TOO_LARGE";
    }
  }
  printf("OpenGL error: %s at %s:%d\n", error, file, line);
}


#define print_gl_errors() _print_gl_errors(__FILE__, __LINE__)

b32
_print_gl_errors(const char *file, u32 line)
{
  b32 success = true;

  GLenum error = glGetError();
  while (error != GL_NO_ERROR)
  {
    success &= false;
    gl_print_error(error, file, line);

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


POLYMORPHIC_LOGGING_FUNCTION(
void
extend_gl_buffer,
{
  if (buffer->total_elements == 0)
  {
    buffer->elements_used = 0;
    buffer->total_elements = INITIAL_GL_BUFFER_TOTAL_ELEMENTS;

    log(channel, "Initial buffer size is 0, allocating new buffer of size %u", buffer->total_elements);

    glBindBuffer(buffer->binding_target, buffer->id);
    glBufferData(buffer->binding_target, buffer->element_size * buffer->total_elements, NULL, GL_STATIC_DRAW);
    glBindBuffer(buffer->binding_target, 0);
  }
  else
  {
    log(channel, "Out of space in buffer, allocating larger buffer.");

    u32 buffer_size = buffer->element_size * buffer->total_elements;
    assert(buffer_size < MAX_U32 / 2);

    u32 new_total_elements = buffer->total_elements * 2;

    GLuint new_buffer = create_buffer();
    glBindBuffer(buffer->binding_target, new_buffer);
    glBufferData(buffer->binding_target, buffer->element_size * new_total_elements, NULL, GL_STATIC_DRAW);

    // Copy data into new VBO
    // Bind buffers to special READ/WRITE copying buffers
    glBindBuffer(GL_COPY_READ_BUFFER, buffer->id);
    glBindBuffer(GL_COPY_WRITE_BUFFER, new_buffer);

    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, buffer->element_size * buffer->elements_used);

    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    glDeleteBuffers(1, &buffer->id);

    buffer->id = new_buffer;
    buffer->total_elements = new_total_elements;

    // Re-setup attributes
    buffer->setup_attributes_func(buffer);

    glBindBuffer(buffer->binding_target, 0);
  }

  print_gl_errors();
  log(channel, "Reallocated VBO.");
},
OpenGL_Buffer *buffer)


POLYMORPHIC_LOGGING_FUNCTION(
void
update_buffer_element,
{
  if (element_position < buffer->elements_used)
  {
    glBindBuffer(buffer->binding_target, buffer->id);
    glBufferSubData(buffer->binding_target, buffer->element_size * element_position, buffer->element_size, new_element);
    glBindBuffer(buffer->binding_target, 0);
  }
  else
  {
    log(channel, "Cannot update element %u. Buffer only has %u elements", element_position, buffer->elements_used);
  }
  print_gl_errors();
},
OpenGL_Buffer *buffer, u32 element_position, void *new_element)


POLYMORPHIC_LOGGING_FUNCTION(
u32
new_buffer_element,
{
  log(channel, "Adding new element.");

  if (buffer->elements_used >= buffer->total_elements)
  {
    extend_gl_buffer(channel, buffer);
  }

  u32 position = buffer->elements_used;
  ++buffer->elements_used;

  update_buffer_element(channel, buffer, position, element);

  print_gl_errors();
  return position;
},
OpenGL_Buffer *buffer, void *element)


void
remove_buffer_element(OpenGL_Buffer *buffer, u32 element_to_remove)
{
  if (element_to_remove < buffer->elements_used &&
      element_to_remove != INVALID_GL_BUFFER_ELEMENT_POSITION)
  {
    glBindBuffer(buffer->binding_target, buffer->id);

    // Reduce size of buffer by one, this is also now indicating the position of the element we need to move into the removed element's slot.
    --buffer->elements_used;
    u32 element_to_move = buffer->elements_used;

    if (element_to_move == 0 ||
        element_to_remove == element_to_move)
    {
      // All elements have been removed, or the element we were removing was at the end of the array, hence no more action is needed.
    }
    else
    {
      // Copy last element into the removed element's slot
      u32 element_to_remove_position = element_to_remove * buffer->element_size;
      u32 element_to_move_position = element_to_move * buffer->element_size;

      glCopyBufferSubData(buffer->binding_target, buffer->binding_target, element_to_move_position, element_to_remove_position, buffer->element_size);
    }

    glBindBuffer(buffer->binding_target, 0);
  }
  print_gl_errors();
}


void *
get_element_from_buffer(OpenGL_Buffer *buffer, u32 element_position)
{
  void *element = malloc(buffer->element_size);

  glBindBuffer(buffer->binding_target, buffer->id);
  glGetBufferSubData(buffer->binding_target, buffer->element_size * element_position, buffer->element_size, element);
  glBindBuffer(buffer->binding_target, 0);

  return element;
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


void
opengl_debug_output_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
  const char *source_str = 0;
  const char *type_str = 0;
  const char * severity_str = 0;

  switch (source)
  {
      case GL_DEBUG_SOURCE_API:
      {
        source_str = "GL_DEBUG_SOURCE_API";
      } break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      {
        source_str = "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
      } break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER:
      {
        source_str = "GL_DEBUG_SOURCE_SHADER_COMPILER";
      } break;
      case GL_DEBUG_SOURCE_THIRD_PARTY:
      {
        source_str = "GL_DEBUG_SOURCE_THIRD_PARTY";
      } break;
      case GL_DEBUG_SOURCE_APPLICATION:
      {
        source_str = "GL_DEBUG_SOURCE_APPLICATION";
      } break;
      case GL_DEBUG_SOURCE_OTHER:
      {
        source_str = "GL_DEBUG_SOURCE_OTHER";
      } break;
  }

  switch (type)
  {
      case GL_DEBUG_TYPE_ERROR:
      {
        type_str = "GL_DEBUG_TYPE_ERROR";
      } break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      {
        type_str = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
      } break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      {
        type_str = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
      } break;
      case GL_DEBUG_TYPE_PORTABILITY:
      {
        type_str = "GL_DEBUG_TYPE_PORTABILITY";
      } break;
      case GL_DEBUG_TYPE_PERFORMANCE:
      {
        type_str = "GL_DEBUG_TYPE_PERFORMANCE";
      } break;
      case GL_DEBUG_TYPE_MARKER:
      {
        type_str = "GL_DEBUG_TYPE_MARKER";
      } break;
      case GL_DEBUG_TYPE_PUSH_GROUP:
      {
        type_str = "GL_DEBUG_TYPE_PUSH_GROUP";
      } break;
      case GL_DEBUG_TYPE_POP_GROUP:
      {
        type_str = "GL_DEBUG_TYPE_POP_GROUP";
      } break;
      case GL_DEBUG_TYPE_OTHER:
      {
        type_str = "GL_DEBUG_TYPE_OTHER";
      } break;
  }

  switch (severity)
  {
      case GL_DEBUG_SEVERITY_HIGH:
      {
        severity_str = "GL_DEBUG_SEVERITY_HIGH";
      } break;
      case GL_DEBUG_SEVERITY_MEDIUM:
      {
        severity_str = "GL_DEBUG_SEVERITY_MEDIUM";
      } break;
      case GL_DEBUG_SEVERITY_LOW:
      {
        severity_str = "GL_DEBUG_SEVERITY_LOW";
      } break;
      case GL_DEBUG_SEVERITY_NOTIFICATION:
      {
        severity_str = "GL_DEBUG_SEVERITY_NOTIFICATION";
      } break;
  }

  log(L_OpenGL_Debug, "Source: %s, Type: %s, ID: %d, Severity: %s, \"%.*s\"", source_str, type_str, id, severity_str, length, message);
}