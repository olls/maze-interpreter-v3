void
draw_box_outline(RenderOperations *render_ops, RenderBasis *render_basis, Rectangle box, V4 color, s32 thickness = 1)
{
  for (s32 i = 0; i < abs(thickness); ++i)
  {
    Rectangle this_box = grow(box, -i*sign(thickness));
    add_line_to_render_list(render_ops, render_basis, this_box.start, (V2){this_box.start.x, this_box.end.y}, color);
    add_line_to_render_list(render_ops, render_basis, this_box.start, (V2){this_box.end.x, this_box.start.y}, color);
    add_line_to_render_list(render_ops, render_basis, (V2){this_box.start.x, this_box.end.y}, this_box.end, color);
    add_line_to_render_list(render_ops, render_basis, (V2){this_box.end.x, this_box.start.y}, this_box.end, color);
  }
}
