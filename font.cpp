void
draw_char(FrameBuffer *frame_buffer, RenderBasis *render_basis, Bitmap *font, V2 world_pos, char x)
{
  u32 char_index = x - 32;
  V2 bitmap_pos = {
    .x = char_index % 16,
    .y = 5 - (char_index / 16)
  };

  V2 char_size = {60, 111};

  Rectangle char_box = {
    .start = bitmap_pos * char_size,
    .end = bitmap_pos * char_size + char_size
  };

  blit_bitmap(frame_buffer, render_basis, font, world_pos, char_box, (V4){1, 1, 1, 1}, 0, true);
}


void
draw_string(FrameBuffer *frame_buffer, RenderBasis *render_basis, Bitmap *font, V2 world_pos, const char str[])
{
  u32 i = 0;
  while (str[i])
  {
    draw_char(frame_buffer, render_basis, font, world_pos, str[i]);

    world_pos.x += 60 * render_basis->world_per_pixel;
    ++i;
  }
}