void
re_form_world_coord(WorldSpace *result)
{
  // Keeps world coordinate correctly formed:
  // - cell_{x,y} between 0 - 2^32
  // - offset between 0 - 1

  vec2 integer_offset = round_down(result->offset);

  if ((integer_offset.x < 0 && result->cell_x > 0) ||
      (integer_offset.x > 0 && result->cell_x < (2^32)-1))
  {
    result->cell_x += integer_offset.x;
    result->offset.x -= integer_offset.x;
  }

  if ((integer_offset.y < 0 && result->cell_y > 0) ||
      (integer_offset.y > 0 && result->cell_y < (2^32)-1))
  {
    result->cell_y += integer_offset.y;
    result->offset.y -= integer_offset.y;
  }
}


vec2
world_coord_to_render_window_coord(RenderWindow *render_window, WorldSpace world_coord)
{
  vec2 result;

  // Recentre the world coordinate about the centre of the window, allowing the offset to be added without loss of precision (see header file comment)

  s64 recentred_cell_x = (s64)world_coord.cell_x - (s64)render_window->render_origin.cell_x;
  s64 recentred_cell_y = (s64)world_coord.cell_y - (s64)render_window->render_origin.cell_y;
  vec2 recentred_cell_coord = (vec2){(r32)recentred_cell_x, (r32)recentred_cell_y}; // Cells at the extreme distance from the render_origin are cropped here

  result = recentred_cell_coord - render_window->render_origin.offset + world_coord.offset;

  return result;
}


vec2
world_coord_to_render_window_coord(RenderWindow *render_window, u32 cell_coord_x, u32 cell_coord_y)
{
  vec2 result = world_coord_to_render_window_coord(render_window, (WorldSpace){cell_coord_x, cell_coord_y, (vec2){0, 0}});
  return result;
}


WorldSpace
render_window_coord_to_world_coord(RenderWindow *render_window, vec2 render_window_coord)
{
  WorldSpace result;

  result.cell_x = render_window->render_origin.cell_x + render_window_coord.x;
  result.cell_y = render_window->render_origin.cell_y + render_window_coord.y;

  result.offset = render_window_coord - round_down(render_window_coord);

  return result;
}