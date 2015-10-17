void
set_pixel(PixelColor * pixels, uint32_t pixel_x, uint32_t pixel_y, V4 color)
{
  uint32_t pixel_pos = (pixel_y * WINDOW_WIDTH) + pixel_x;

  V3 prev_color = pixel_color_to_V3(pixels[pixel_pos]);
  V3 new_color = remove_alpha(color);

  PixelColor alpha_blended = to_color((color.a * new_color) + ((1.0f - color.a) * prev_color));
  pixels[pixel_pos] = alpha_blended;
}


void
draw_circle(PixelColor * pixels,
            Rectangle render_region,
            V2 world_pos,
            float world_radius,
            V4 color)
{
  // Into fractional pixel space
  V2 fract_pixel_pos = world_pos * WORLD_COORDS_TO_PIXELS;

  float radius = world_radius * WORLD_COORDS_TO_PIXELS;
  float radius_sq = squared(radius);
  float radius_minus_one_sq = squared(radius - 1);

  Rectangle render_region_pixels = render_region * WORLD_COORDS_TO_PIXELS;
  Rectangle window_region_pixels = (Rectangle){(V2){0, 0}, (V2){WINDOW_WIDTH, WINDOW_HEIGHT}};

  Rectangle fract_pixels_circle_region = {fract_pixel_pos - radius,
                                          fract_pixel_pos + radius};
  fract_pixels_circle_region = crop_to(fract_pixels_circle_region, render_region_pixels);
  fract_pixels_circle_region = crop_to(fract_pixels_circle_region, window_region_pixels);

  Rectangle pixels_circle_region = round_down(fract_pixels_circle_region);

  for (uint32_t pixel_y = pixels_circle_region.start.y;
       pixel_y < pixels_circle_region.end.y;
       pixel_y++)
  {
    for (uint32_t pixel_x = pixels_circle_region.start.x;
         pixel_x < pixels_circle_region.end.x;
         pixel_x++)
    {
      V2 d_center = (V2){pixel_x, pixel_y} - fract_pixel_pos;
      float dist_sq = length_sq(d_center);

      if (dist_sq < radius_sq)
      {
        V4 this_color = color;

        if (dist_sq > radius_minus_one_sq)
        {
          float diff = radius - sqrt(dist_sq);
          this_color.a *= diff;
        }

        set_pixel(pixels, pixel_x, pixel_y, this_color);
      }
    }
  }
}


void
draw_box(PixelColor * pixels,
         Rectangle render_region,
         Rectangle box,
         V4 color)
{
  // Into fractional pixel space
  Rectangle fract_pixel_space = box * WORLD_COORDS_TO_PIXELS;

  Rectangle render_region_pixels = render_region * WORLD_COORDS_TO_PIXELS;
  Rectangle window_region_pixels = (Rectangle){(V2){0, 0}, (V2){WINDOW_WIDTH, WINDOW_HEIGHT}};
  fract_pixel_space = crop_to(fract_pixel_space, render_region_pixels);
  fract_pixel_space = crop_to(fract_pixel_space, window_region_pixels);

  Rectangle pixel_space = round_down(fract_pixel_space);

  for (uint32_t pixel_y = pixel_space.start.y;
       pixel_y < pixel_space.end.y;
       pixel_y++)
  {
    for (uint32_t pixel_x = pixel_space.start.x;
         pixel_x < pixel_space.end.x;
         pixel_x++)
    {
      V4 this_color = color;

      if (pixel_x == pixel_space.start.x)
      {
        this_color.a *= (pixel_space.start.x + 1) - fract_pixel_space.start.x;
      }
      if (pixel_x == pixel_space.end.x)
      {
        this_color.a *= fract_pixel_space.end.x - pixel_space.end.x;
      }
      if (pixel_y == pixel_space.start.y)
      {
        this_color.a *= (pixel_space.start.y + 1) - fract_pixel_space.start.y;
      }
      if (pixel_y == pixel_space.end.y)
      {
        this_color.a *= fract_pixel_space.end.y - pixel_space.end.y;
      }

      set_pixel(pixels, pixel_x, pixel_y, this_color);
    }
  }
}