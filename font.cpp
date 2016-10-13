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

  BlitBitmapOptions blit_opts;
  get_default_blit_bitmap_options(&blit_opts);
  blit_opts.crop = char_box;
  blit_opts.interpolation = true;
  blit_bitmap(frame_buffer, render_basis, font, world_pos, &blit_opts);
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


void
draw_formatted_string(FrameBuffer *frame_buffer, RenderBasis *render_basis, Bitmap *font, V2 world_pos, const char *text, ...)
{
  char buf[256];
  va_list aptr;
  va_start(aptr, text);
  vsnprintf(buf, 256, text, aptr);
  va_end(aptr);

  draw_string(frame_buffer, render_basis, font, world_pos, buf);
}
