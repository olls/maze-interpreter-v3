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
draw_circle(GameSetup * setup,
            PixelColor * pixels,
            Rectangle render_region_world,
            V2 world_pos,
            float world_radius,
            V4 color)
{
  // TODO: There seems to be some off-by-one bug in here, the right
  //       side of the circle seems to be clipped slightly sometimes.

  V2 fract_pixel_pos = world_pos * (1.0f / setup->world_per_pixel);

  float radius = world_radius * (1.0f / setup->world_per_pixel);
  float radius_sq = squared(radius);
  float radius_minus_one_sq = squared(radius - 1);

  Rectangle window_region = (Rectangle){(V2){0, 0}, (V2){WINDOW_WIDTH, WINDOW_HEIGHT}};
  Rectangle render_region = render_region_world * (1.0f / setup->world_per_pixel);
  render_region = get_overlap(render_region, window_region);

  Rectangle fract_pixels_circle_region = {fract_pixel_pos - radius,
                                          fract_pixel_pos + radius};
  fract_pixels_circle_region = crop_to(fract_pixels_circle_region, render_region);

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
draw_box(GameSetup * setup, PixelColor * pixels, Rectangle render_region_world, Rectangle box, V4 color)
{
  Rectangle window_region = (Rectangle){(V2){0, 0}, (V2){WINDOW_WIDTH, WINDOW_HEIGHT}};
  Rectangle render_region = render_region_world * (1.0f / setup->world_per_pixel);
  render_region = get_overlap(render_region, window_region);

  Rectangle fract_pixel_space = box * (1.0f / setup->world_per_pixel);
  fract_pixel_space = crop_to(fract_pixel_space, render_region);

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


void
draw_line(GameSetup * setup, PixelColor * pixels, Rectangle render_region_world, V2 world_start, V2 world_end, V4 color)
{
  V2 start = world_start * (1.0f / setup->world_per_pixel);
  V2 end = world_end * (1.0f / setup->world_per_pixel);

  if (start.x > end.x)
  {
    printf("Swap\n");
    V2 temp = start;
    start = end;
    end = temp;
  }

  Rectangle window_region = (Rectangle){(V2){0, 0}, (V2){WINDOW_WIDTH, WINDOW_HEIGHT}};
  Rectangle render_region = render_region_world * (1.0f / setup->world_per_pixel);
  render_region = crop_to(render_region, window_region);

  bool start_in_region = in_rectangle(start, render_region);
  bool end_in_region = in_rectangle(end, render_region);

  if (start_in_region || end_in_region)
  {
    float x_gradient = (end.y - start.y) / (end.x - start.x);
    float y_gradient = (end.x - start.x) / (end.y - start.y);
    if (!start_in_region)
    {
      if (start.x < render_region.start.x)
      {
        start.y += (render_region.start.x - start.x) * x_gradient;
        start.x = render_region.start.x;
      }
      else if (start.x >= render_region.end.x)
      {
        start.y += (render_region.end.x - start.x) * x_gradient;
        start.x = render_region.end.x;
      }
      if (start.y < render_region.start.y)
      {
        start.x += (render_region.start.y - start.y) * y_gradient;
        start.y = render_region.start.y;
      }
      else if (start.y >= render_region.end.y)
      {
        start.x += (render_region.end.y - start.y) * y_gradient;
        start.y = render_region.end.y;
      }
    }
    if (!end_in_region)
    {
      if (end.x < render_region.start.x)
      {
        end.y += (render_region.start.x - end.x) * x_gradient;
        end.x = render_region.end.x;
      }
      else if (end.x >= render_region.end.x)
      {
        end.y += (render_region.end.x - end.x) * x_gradient;
        end.x = render_region.end.x;
      }
      if (end.y < render_region.start.y)
      {
        end.x += (render_region.start.y - end.y) * y_gradient;
        end.y = render_region.end.y;
      }
      else if (end.y >= render_region.end.y)
      {
        end.x += (render_region.end.y - end.y) * y_gradient;
        end.y = render_region.end.y;
      }
    }

    // TODO: IMPORTANT: Sub-pixel rendering!!!

    V2 length_components = {(end.x - start.x),
                            (end.y - start.y)};

    float num_pixels = fmax(abs(length_components.x), abs(length_components.y));

    if(num_pixels)
    {
      V2 step = length_components / num_pixels;

      V2 pixel_pos_fract = start;
      for (uint32_t pixel_n = 0;
           pixel_n < num_pixels;
           ++pixel_n)
      {
        V2 pixel_pos = round_down(pixel_pos_fract);
        set_pixel(pixels, pixel_pos.x, pixel_pos.y, color);
        pixel_pos_fract += step;
      }
    }
  }
}


void
draw_box_outline(GameSetup * setup, PixelColor * pixels, Rectangle render_region_world, Rectangle box, V4 color)
{
  draw_line(setup, pixels, render_region_world, box.start, (V2){box.start.x, box.end.y}, color);
  draw_line(setup, pixels, render_region_world, box.start, (V2){box.end.x, box.start.y}, color);
  draw_line(setup, pixels, render_region_world, (V2){box.start.x, box.end.y}, box.end, color);
  draw_line(setup, pixels, render_region_world, (V2){box.end.x, box.start.y}, box.end, color);
}