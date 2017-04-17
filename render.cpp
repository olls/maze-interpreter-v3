void
set_pixel(Renderer *renderer, u32 pixel_x, u32 pixel_y, V4 color)
{
  b32 x_on_screen = pixel_x < renderer->frame_buffer.width;
  b32 y_on_screen = pixel_y < renderer->frame_buffer.height;

#ifdef DEBUG
  assert(x_on_screen);
  assert(y_on_screen);
#else
  if (!x_on_screen || !y_on_screen)
  {
    return;
  }
#endif

  u32 pixel_pos = (pixel_y * renderer->frame_buffer.width) + pixel_x;

  V3 prev_color = pixel_color_to_V3(renderer->frame_buffer.buffer[pixel_pos]);
  V3 new_color = remove_alpha(color) * 255.0;

  PixelColor alpha_blended = to_color((color.a * new_color) + ((1.0f - color.a) * prev_color));
  renderer->frame_buffer.buffer[pixel_pos] = alpha_blended;
}


void
render_circle(Renderer *renderer,
              RenderBasis *render_basis,
              V2 world_pos,
              r32 world_radius,
              V4 color,
              r32 world_outline_width = -1,
              u32 corners = 0b1111)
{
  // TODO: There seems to be some off-by-one bug in here, the right /
  //       bottom side of the circle seems to be clipped slightly
  //       sometimes.

  V2 fract_pixel_pos = transform_coord(render_basis, world_pos);

  r32 radius = (world_radius / render_basis->world_per_pixel) * render_basis->scale;
  r32 radius_sq = squared(radius);
  r32 radius_minus_one_sq = squared(radius - 1);

  r32 inner_radius;
  r32 inner_radius_sq;
  r32 inner_radius_minus_one_sq;
  b32 outline = false;
  if (world_outline_width > 0)
  {
    r32 outline_width = (world_outline_width / render_basis->world_per_pixel) * render_basis->scale;
    if (outline_width < radius)
    {
      inner_radius = radius - outline_width;
      inner_radius_sq = squared(inner_radius);
      inner_radius_minus_one_sq = squared(inner_radius - 1);

      outline = true;
    }
  }

  Rectangle window_region = (Rectangle){(V2){0, 0}, (V2){renderer->frame_buffer.width, renderer->frame_buffer.height}};
  Rectangle render_region = render_basis->clip_region / render_basis->world_per_pixel;
  render_region = get_overlap(render_region, window_region);

  Rectangle fract_pixels_circle_region = {fract_pixel_pos, fract_pixel_pos};
  if (corners & 0b1000)
  {
    fract_pixels_circle_region = get_union(fract_pixels_circle_region,
                                           (Rectangle){(V2){fract_pixel_pos.x,              fract_pixel_pos.y - radius},
                                                       (V2){fract_pixel_pos.x + radius + 1, fract_pixel_pos.y         }});
  }
  if (corners & 0b0100)
  {
    fract_pixels_circle_region = get_union(fract_pixels_circle_region,
                                           (Rectangle){fract_pixel_pos,
                                                       fract_pixel_pos + radius + 1});
  }
  if (corners & 0b0010)
  {
    fract_pixels_circle_region = get_union(fract_pixels_circle_region,
                                           (Rectangle){(V2){fract_pixel_pos.x - radius, fract_pixel_pos.y             },
                                                       (V2){fract_pixel_pos.x,          fract_pixel_pos.y + radius + 1}});
  }
  if (corners & 0b0001)
  {
    fract_pixels_circle_region = get_union(fract_pixels_circle_region,
                                           (Rectangle){fract_pixel_pos - radius,
                                                       fract_pixel_pos + 1});
  }

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

      if (dist_sq < radius_sq &&
          (!outline || dist_sq > inner_radius_minus_one_sq))
      {
        V4 this_color = color;

        if (dist_sq > radius_minus_one_sq)
        {
          r32 diff = radius - sqrt(dist_sq);
          this_color.a *= diff;
        }

        if (outline && dist_sq < inner_radius_sq)
        {
          r32 diff = 1 - (inner_radius - sqrt(dist_sq));
          this_color.a *= diff;
        }

        set_pixel(renderer, pixel_x, pixel_y, this_color);
      }
    }
  }
}


void
render_box(Renderer *renderer, RenderBasis *render_basis, Rectangle box, V4 color)
{
  Rectangle window_region = (Rectangle){(V2){0, 0}, (V2){renderer->frame_buffer.width, renderer->frame_buffer.height}};
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

      set_pixel(renderer, pixel_x, pixel_y, this_color);
    }
  }
}


void
fast_render_box(Renderer *renderer, RenderBasis *render_basis, Rectangle box, PixelColor color)
{
  Rectangle window_region = (Rectangle){(V2){0, 0}, (V2){renderer->frame_buffer.width, renderer->frame_buffer.height}};
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
      renderer->frame_buffer.buffer[pixel_x + pixel_y * renderer->frame_buffer.width] = color;
    }
  }
}


void
render_line(Renderer *renderer, RenderBasis *render_basis, V2 world_start, V2 world_end, V4 color, r32 world_width, LineEndStyle start_line_end, LineEndStyle end_line_end)
{
  V2 start = transform_coord(render_basis, world_start);
  V2 end = transform_coord(render_basis, world_end);
  r32 width = render_basis->scale * world_width / render_basis->world_per_pixel;

  // Extend the line for square line ends. (This is done before we swap start and end points so the start and end points retain their link with the end styles)
  V2 initial_length_components = {(end.x - start.x),
                                  (end.y - start.y)};
  V2 initial_direction = unit_vector(initial_length_components);

  if (start_line_end == LINE_END_SQUARE)
  {
    // Extend line by width/2 at start
    V2 width_vector = width * 0.5 * initial_direction;

    start -= width_vector;
  }

  if (end_line_end == LINE_END_SQUARE)
  {
    // Extend line by width/2 at end
    V2 width_vector = width * 0.5 * initial_direction;

    end += width_vector;
  }

  // Swap start and end around so we can render in the same direction
  if (start.x > end.x)
  {
    V2 tmp = start;
    start = end;
    end = tmp;
  }

  Rectangle window_region = (Rectangle){(V2){0, 0}, (V2){renderer->frame_buffer.width, renderer->frame_buffer.height}};
  Rectangle render_region = render_basis->clip_region / render_basis->world_per_pixel;
  render_region = crop_to(render_region, window_region);

  // Clip line endpoints to screen
  // TODO: This doesn't really behave correctly when width > 1

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

  // Calculate line width stuff

  V2 length_components = {(end.x - start.x),
                          (end.y - start.y)};

  r32 x_length = abs(length_components.x);
  r32 y_length = abs(length_components.y);

  r32 theta = atan2f(length_components.y, length_components.x);

  V2 width_components = {abs(width / (r32)sin(theta)),
                         abs(width / (r32)sin((M_PI*.5)-theta))};

  // Extension for the butt end caps.
  r32 extenstion;
  if (x_length > y_length)
  {
    extenstion = width * abs((r32)tan(theta)) * 0.5;
  }
  else
  {
    extenstion = width * abs((r32)tan((M_PI*.5) - theta)) * 0.5;
  }

  V2 direction = unit_vector(length_components);
  V2 extenstion_vector = extenstion * direction;
  start -= extenstion_vector;
  end   += extenstion_vector;

  length_components += extenstion_vector * 2;
  x_length += abs(extenstion_vector.x) * 2;
  y_length += abs(extenstion_vector.y) * 2;

  r32 num_steps;
  r32 width_comp;
  V2 drawing_axis;
  r32 extenstion_comp;

  if (x_length > y_length)
  {
    num_steps = x_length;
    drawing_axis = (V2){0, 1};
    width_comp = width_components.y;
    extenstion_comp = abs(extenstion_vector.x);
  }
  else
  {
    num_steps = y_length;
    drawing_axis = (V2){1, 0};
    width_comp = width_components.x;
    extenstion_comp = abs(extenstion_vector.y);
  }

  // TODO: Sub-pixel rendering

  if (num_steps)
  {
    V2 step = length_components / num_steps;

    V2 line_center_fract = start;
    for (u32 pixel_n = 0;
         pixel_n < num_steps;
         ++pixel_n)
    {
      r32 end_slant_comp = extenstion_comp*2;
      if (pixel_n <= end_slant_comp || pixel_n >= num_steps - end_slant_comp)
      {
        // End caps
        u32 d;
        b32 start;
        if (pixel_n <= end_slant_comp)
        {
          start = true;
          d = abs(end_slant_comp - pixel_n);
        }
        else
        {
          start = false;
          d = abs((num_steps - end_slant_comp) - pixel_n);
        }

        b32 flip_axis = start;
        if (drawing_axis.y == 1 && length_components.y < 0)
        {
          flip_axis = !start;
        }

        V2 this_cap_drawing_axis;
        if (flip_axis)
        {
          this_cap_drawing_axis = -drawing_axis;
        }
        else
        {
          this_cap_drawing_axis = drawing_axis;
        }

        r32 this_width_comp = width_comp * (1 - (d / end_slant_comp));

        r32 width_start_line_center_offset = width_comp * 0.5;
        V2 pixel_pos_fract = line_center_fract - (width_start_line_center_offset * this_cap_drawing_axis);

        for (u32 offset = 0;
             offset < this_width_comp;
             ++offset)
        {
          V2 pixel_pos = round_down(pixel_pos_fract);
          set_pixel(renderer, pixel_pos.x, pixel_pos.y, color);

          pixel_pos_fract += this_cap_drawing_axis;
        }
      }
      else
      {
        // Main line bulk

        r32 width_start_line_center_offset = width_comp * 0.5;
        V2 pixel_pos_fract = line_center_fract - (width_start_line_center_offset * drawing_axis);

        for (u32 offset = 0;
             offset < width_comp;
             ++offset)
        {
          V2 pixel_pos = round_down(pixel_pos_fract);
          set_pixel(renderer, pixel_pos.x, pixel_pos.y, color);

          pixel_pos_fract += drawing_axis;
        }
      }

      line_center_fract += step;
    }
  }

  // Draw circles
  r32 world_radius = world_width * 0.5;

  if (start_line_end == LINE_END_ROUND)
  {
    render_circle(renderer, render_basis, world_start, world_radius, color);
  }
  if (end_line_end == LINE_END_ROUND)
  {
    render_circle(renderer, render_basis, world_end, world_radius, color);
  }
}


void
render_rotated_box(Renderer *renderer, RenderBasis *render_basis, Rectangle world_box, V2 world_rotation_origin, r32 theta, V4 colour)
{
  transform_coord_rect(render_basis, world_box);
}