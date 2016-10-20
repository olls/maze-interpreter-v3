void
add_box_outline_to_render_list(RenderOperations *render_ops, RenderBasis *render_basis, Rectangle box, V4 color, r32 thickness = 1)
{
  add_box_to_render_list(render_ops, render_basis, (Rectangle){box.start, (V2){box.end.x, box.start.y+thickness}}, color);
  add_box_to_render_list(render_ops, render_basis, (Rectangle){(V2){box.start.x, box.end.y-thickness}, box.end}, color);
  add_box_to_render_list(render_ops, render_basis, (Rectangle){(V2){box.start.x, box.start.y+thickness}, (V2){box.start.x+thickness, box.end.y-thickness}}, color);
  add_box_to_render_list(render_ops, render_basis, (Rectangle){(V2){box.end.x-thickness, box.start.y+thickness}, (V2){box.end.x, box.end.y-thickness}}, color);
}
