void
get_orthographic_basis(RenderBasis *result, Rectangle render_region_pixels)
{
  zero(result, RenderBasis);
  result->origin = render_region_pixels.start;
  result->world_per_pixel = 1;
  result->scale = 1;
  result->clip_region = render_region_pixels;
}


V2
transform_coord(RenderBasis *render_basis, V2 map_coord_world)
{
  // This is having problems with zooming...

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

  V2 scaled_d_coord_scale_focus_world = scaled_screen_coord_world - render_basis->scale_focus;

  V2 d_map_coord_scale_focus_world = scaled_d_coord_scale_focus_world / render_basis->scale;
  V2 map_coord_world = d_map_coord_scale_focus_world + render_basis->scale_focus;

  return map_coord_world;
}


Rectangle
transform_coord_rect(RenderBasis *render_basis, Rectangle rect)
{
  Rectangle result;

  result.start = transform_coord(render_basis, rect.start);
  result.end = transform_coord(render_basis, rect.end);

  return result;
}
