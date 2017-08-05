void
draw_char(Bitmap *font, vec2 world_pos, u8 x, r32 scale = 1, vec4 color = (vec4){-1})
{
  u32 char_index = x - MIN_CHAR;
  vec2 bitmap_pos = {
    .x = r32(char_index % 16),
    .y = 5 - r32(char_index / 16)
  };

  Rectangle char_box = {
    .start = bitmap_pos * CHAR_SIZE,
    .end = bitmap_pos * CHAR_SIZE + CHAR_SIZE
  };

  // BlitBitmapOptions blit_opts;
  // get_default_blit_bitmap_options(&blit_opts);
  // blit_opts.crop = char_box;
  // blit_opts.interpolation = true;
  // blit_opts.scale = scale;
  // if (color.a != -1)
  // {
  //   blit_opts.color_multiplier = color;
  // }
  // draw_bitmap(font, world_pos, &blit_opts);
}


void
draw_string(Bitmap *font, vec2 world_pos, const u8 str[], r32 scale = 1, vec4 color = (vec4){-1})
{
  u32 i = 0;
  while (str[i])
  {
    draw_char(font, world_pos, str[i], scale, color);

    world_pos.x += CHAR_SIZE.x * scale;
    ++i;
  }
}


u32
formatted_string(u8 *out, u32 max_len, const u8 *pattern,  ...)
{
  u32 result;

  va_list aptr;
  va_start(aptr, pattern);
  s32 written = vsnprintf((char *)out, max_len, (const char *)pattern, aptr);
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
