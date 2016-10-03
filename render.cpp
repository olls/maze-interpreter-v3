void
set_pixel(FrameBuffer *frame_buffer, u32 pixel_x, u32 pixel_y, V4 color)
{
  assert(pixel_x >= 0 && pixel_x < frame_buffer->width);
  assert(pixel_y >= 0 && pixel_y < frame_buffer->height);

  u32 pixel_pos = (pixel_y * frame_buffer->width) + pixel_x;

  V3 prev_color = pixel_color_to_V3(frame_buffer->buffer[pixel_pos]);
  V3 new_color = remove_alpha(color) * 255.0;

  PixelColor alpha_blended = to_color((color.a * new_color) + ((1.0f - color.a) * prev_color));
  frame_buffer->buffer[pixel_pos] = alpha_blended;
}


void
get_orthographic_basis(RenderBasis *result, FrameBuffer *frame_buffer)
{
  Rectangle render_region_pixels;
  render_region_pixels.start = (V2){0, 0};
  render_region_pixels.end = (V2){frame_buffer->width, frame_buffer->height};

  result->origin = (V2){0 ,0};
  result->world_per_pixel = 1;
  result->scale = 1;
  result->clip_region = render_region_pixels;
}


V2
transform_coord(RenderBasis *render_basis, V2 map_coord_world)
{
  V2 d_map_coord_scale_focus_world = map_coord_world - render_basis->scale_focus;
  V2 scaled_d_coord_scale_focus_world = render_basis->scale * d_map_coord_scale_focus_world;

  V2 scaled_screen_coord_world = render_basis->scale_focus + scaled_d_coord_scale_focus_world;

  V2 scaled_screen_coord_pixels = (render_basis->origin + scaled_screen_coord_world) / render_basis->world_per_pixel;

  return scaled_screen_coord_pixels;
}


V2
untransform_coord(RenderBasis *render_basis, V2 scaled_screen_coord_pixels)
{
  V2 scaled_screen_coord_world = (scaled_screen_coord_pixels * render_basis->world_per_pixel) - render_basis->origin;

  V2 scaled_d_coord_scale_focus_world = render_basis->scale_focus - scaled_screen_coord_world;

  V2 d_map_coord_scale_focus_world = scaled_d_coord_scale_focus_world / render_basis->scale;
  V2 map_coord_world = d_map_coord_scale_focus_world + render_basis->scale_focus;

  return scaled_screen_coord_world;
}


Rectangle
transform_coord_rect(RenderBasis *render_basis, Rectangle rect)
{
  Rectangle result;

  result.start = transform_coord(render_basis, rect.start);
  result.end = transform_coord(render_basis, rect.end);

  return result;
}


void
draw_circle(FrameBuffer *frame_buffer,
            RenderBasis *render_basis,
            V2 world_pos,
            r32 world_radius,
            V4 color)
{
  // TODO: There seems to be some off-by-one bug in here, the right /
  //       bottom side of the circle seems to be clipped slightly
  //       sometimes.

  V2 fract_pixel_pos = transform_coord(render_basis, world_pos);

  r32 radius = (world_radius / render_basis->world_per_pixel) * render_basis->scale;
  r32 radius_sq = squared(radius);
  r32 radius_minus_one_sq = squared(radius - 1);

  Rectangle window_region = (Rectangle){(V2){0, 0}, (V2){frame_buffer->width, frame_buffer->height}};
  Rectangle render_region = render_basis->clip_region / render_basis->world_per_pixel;
  render_region = get_overlap(render_region, window_region);

  Rectangle fract_pixels_circle_region = {fract_pixel_pos - radius,
                                          fract_pixel_pos + radius + 1};
  fract_pixels_circle_region = crop_to(fract_pixels_circle_region, render_region);

  Rectangle pixels_circle_region = round_down(fract_pixels_circle_region);

  for (u32 pixel_y = pixels_circle_region.start.y;
       pixel_y < pixels_circle_region.end.y;
       pixel_y++)
  {
    for (u32 pixel_x = pixels_circle_region.start.x;
         pixel_x < pixels_circle_region.end.x;
         pixel_x++)
    {
      V2 d_center = (V2){pixel_x, pixel_y} - fract_pixel_pos;
      r32 dist_sq = length_sq(d_center);

      if (dist_sq < radius_sq)
      {
        V4 this_color = color;

        if (dist_sq > radius_minus_one_sq)
        {
          r32 diff = radius - sqrt(dist_sq);
          this_color.a *= diff;
        }

        set_pixel(frame_buffer, pixel_x, pixel_y, this_color);
      }
    }
  }
}


void
draw_box(FrameBuffer *frame_buffer, RenderBasis *render_basis, Rectangle box, V4 color)
{
  Rectangle window_region = (Rectangle){(V2){0, 0}, (V2){frame_buffer->width, frame_buffer->height}};
  Rectangle render_region = render_basis->clip_region / render_basis->world_per_pixel;
  render_region = get_overlap(render_region, window_region);

  Rectangle fract_pixel_space = transform_coord_rect(render_basis, box);

  Rectangle pixel_space = round_down(fract_pixel_space);
  pixel_space = (Rectangle){pixel_space.start, pixel_space.end + 1};

  pixel_space = crop_to(pixel_space, render_region);

  for (u32 pixel_y = pixel_space.start.y;
       pixel_y < pixel_space.end.y;
       pixel_y++)
  {
    for (u32 pixel_x = pixel_space.start.x;
         pixel_x < pixel_space.end.x;
         pixel_x++)
    {
      V4 this_color = color;

      if (pixel_x == pixel_space.start.x)
      {
        this_color.a *= (pixel_space.start.x + 1) - fract_pixel_space.start.x;
      }
      if (pixel_x == pixel_space.end.x-1)
      {
        this_color.a *= fract_pixel_space.end.x - (pixel_space.end.x - 1);
      }
      if (pixel_y == pixel_space.start.y)
      {
        this_color.a *= (pixel_space.start.y + 1) - fract_pixel_space.start.y;
      }
      if (pixel_y == pixel_space.end.y-1)
      {
        this_color.a *= fract_pixel_space.end.y - (pixel_space.end.y - 1);
      }

      set_pixel(frame_buffer, pixel_x, pixel_y, this_color);
    }
  }
}


void
fast_draw_box(FrameBuffer *frame_buffer, RenderBasis *render_basis, Rectangle box, PixelColor color)
{
  Rectangle window_region = (Rectangle){(V2){0, 0}, (V2){frame_buffer->width, frame_buffer->height}};
  Rectangle render_region = render_basis->clip_region / render_basis->world_per_pixel;
  render_region = get_overlap(render_region, window_region);

  Rectangle fract_pixel_space = transform_coord_rect(render_basis, box);
  fract_pixel_space = crop_to(fract_pixel_space, render_region);

  Rectangle pixel_space = round_down(fract_pixel_space);

  for (u32 pixel_y = pixel_space.start.y;
       pixel_y < pixel_space.end.y;
       pixel_y++)
  {
    for (u32 pixel_x = pixel_space.start.x;
         pixel_x < pixel_space.end.x;
         pixel_x++)
    {
      frame_buffer->buffer[pixel_x + pixel_y * frame_buffer->width] = color;
    }
  }
}


void
draw_line(FrameBuffer *frame_buffer, RenderBasis *render_basis, V2 world_start, V2 world_end, V4 color)
{
  V2 start = transform_coord(render_basis, world_start);
  V2 end = transform_coord(render_basis, world_end);

  if (start.x > end.x)
  {
    V2 temp = start;
    start = end;
    end = temp;
  }

  Rectangle window_region = (Rectangle){(V2){0, 0}, (V2){frame_buffer->width, frame_buffer->height}};
  Rectangle render_region = render_basis->clip_region / render_basis->world_per_pixel;
  render_region = crop_to(render_region, window_region);

  r32 x_gradient = (end.y - start.y) / (end.x - start.x);
  r32 y_gradient = (end.x - start.x) / (end.y - start.y);

  if (!in_rectangle(start, render_region))
  {
    if (start.x < render_region.start.x)
    {
      start.y += (render_region.start.x - start.x) * x_gradient;
      start.x = render_region.start.x;
    }
    else if (start.x >= render_region.end.x)
    {
      start.y += (render_region.end.x - start.x) * x_gradient;
      start.x = render_region.end.x - 1;
    }
    if (start.y < render_region.start.y)
    {
      start.x += (render_region.start.y - start.y) * y_gradient;
      start.y = render_region.start.y;
    }
    else if (start.y >= render_region.end.y)
    {
      start.x += (render_region.end.y - start.y) * y_gradient;
      start.y = render_region.end.y - 1;
    }
  }
  if (!in_rectangle(end, render_region))
  {
    if (end.x < render_region.start.x)
    {
      end.y += (render_region.start.x - end.x) * x_gradient;
      end.x = render_region.start.x;
    }
    else if (end.x >= render_region.end.x)
    {
      end.y += (render_region.end.x - end.x) * x_gradient;
      end.x = render_region.end.x - 1;
    }
    if (end.y < render_region.start.y)
    {
      end.x += (render_region.start.y - end.y) * y_gradient;
      end.y = render_region.start.y;
    }
    else if (end.y >= render_region.end.y)
    {
      end.x += (render_region.end.y - end.y) * y_gradient;
      end.y = render_region.end.y - 1;
    }
  }

  // TODO: IMPORTANT: Sub-pixel rendering!!!

  V2 length_components = {(end.x - start.x),
                          (end.y - start.y)};

  r32 num_pixels = max(fabs(length_components.x), fabs(length_components.y));

  if(num_pixels)
  {
    V2 step = length_components / num_pixels;

    V2 pixel_pos_fract = start;
    for (u32 pixel_n = 0;
         pixel_n < num_pixels;
         ++pixel_n)
    {
      V2 pixel_pos = round_down(pixel_pos_fract);
      set_pixel(frame_buffer, pixel_pos.x, pixel_pos.y, color);
      pixel_pos_fract += step;
    }
  }
}


void
draw_box_outline(FrameBuffer *frame_buffer, RenderBasis *render_basis, Rectangle box, V4 color)
{
  draw_line(frame_buffer, render_basis, box.start, (V2){box.start.x, box.end.y}, color);
  draw_line(frame_buffer, render_basis, box.start, (V2){box.end.x, box.start.y}, color);
  draw_line(frame_buffer, render_basis, (V2){box.start.x, box.end.y}, box.end, color);
  draw_line(frame_buffer, render_basis, (V2){box.end.x, box.start.y}, box.end, color);
}
