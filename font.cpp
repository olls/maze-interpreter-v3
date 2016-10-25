void
draw_char(RenderOperations *render_operations, RenderBasis *render_basis, Bitmap *font, V2 world_pos, char x, r32 scale = 1, V4 color = (V4){-1})
{
  u32 char_index = x - MIN_CHAR;
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
  add_bitmap_to_render_list(render_operations, render_basis, font, world_pos, &blit_opts);
}


void
draw_string(RenderOperations *render_operations, RenderBasis *render_basis, Bitmap *font, V2 world_pos, const char str[], r32 scale = 1, V4 color = (V4){-1})
{
  u32 i = 0;
  while (str[i])
  {
    draw_char(render_operations, render_basis, font, world_pos, str[i], scale, color);

    world_pos.x += CHAR_SIZE.x * render_basis->world_per_pixel * scale;
    ++i;
  }
}


u32
fmted_str(char *out, u32 max_len, const char *pattern,  ...)
{
  u32 result;

  va_list aptr;
  va_start(aptr, pattern);
  s32 written = vsnprintf(out, max_len, pattern, aptr);
  va_end(aptr);

  if (written >= 0 && written <= max_len)
  {
    result = (u32)written;
  }
  else if (written > max_len)
  {
    result = max_len;
  }
  else
  {
    result = 0;
  }

  return result;
}
