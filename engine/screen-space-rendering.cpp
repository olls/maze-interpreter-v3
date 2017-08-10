b32
setup_screen_space_rendering(ScreenSpaceRendering *screen_space)
{
  b32 success = true;

  const u8 *filenames[] = {u8("screen.glvs"), u8("screen.glfs")};
  GLenum shader_types[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
  success &= create_shader_program(filenames, shader_types, array_count(shader_types), &screen_space->shader_program);

  return success;
}