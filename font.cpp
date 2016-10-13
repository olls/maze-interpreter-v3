V2 CHAR_SIZE = {66, 116};


void
draw_char(FrameBuffer *frame_buffer, RenderBasis *render_basis, Bitmap *font, V2 world_pos, char x, r32 scale = 1, V4 color = (V4){-1})
{
  u32 char_index = x - 32;
  V2 bitmap_pos = {
    .x = char_index % 16,
    .y = 5 - (char_index / 16)
  };

  Rectangle char_box = {
    .start = bitmap_pos * CHAR_SIZE,
    .end = bitmap_pos * CHAR_SIZE + CHAR_SIZE
  };

  BlitBitmapOptions blit_opts;
  get_default_blit_bitmap_options(&blit_opts);
  blit_opts.crop = char_box;
  blit_opts.interpolation = true;
  blit_opts.scale = scale;
  if (color.a != -1)
  {
    blit_opts.color_multiplier = color;
  }
  blit_bitmap(frame_buffer, render_basis, font, world_pos, &blit_opts);
}


void
draw_string(FrameBuffer *frame_buffer, RenderBasis *render_basis, Bitmap *font, V2 world_pos, const char str[], r32 scale = 1, V4 color = (V4){-1})
{
  u32 i = 0;
  while (str[i])
  {
    draw_char(frame_buffer, render_basis, font, world_pos, str[i], scale, color);

    world_pos.x += CHAR_SIZE.x * render_basis->world_per_pixel * scale;
    ++i;
  }
}


void
fmted_str(char out[256], const char *pattern,  ...)
{
  va_list aptr;
  va_start(aptr, pattern);
  vsnprintf(out, 256, pattern, aptr);
  va_end(aptr);
}
