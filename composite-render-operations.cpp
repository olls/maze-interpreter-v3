void
add_box_outline_to_render_list(RenderOperations *render_ops, RenderBasis *render_basis, Rectangle box, V4 color, r32 thickness = 1, r32 radius = 0)
{
  // Square and rounded box are in separate paths because the rounded version with radius = 0 overlaps the corners of the lines, causing the anti-aliasing to double up.

  // Because the circle and box rendering functions do not quite line up, we have to do this to fudge because we are working in world space.
  r32 one_pixel = 1 * render_basis->world_per_pixel / render_basis->scale;

  if (radius > 0)
  {
    add_circle_to_render_list(render_ops, render_basis, (V2){box.end.x - radius, box.start.y + radius - one_pixel}, radius, color, thickness, 0b1000);
    add_circle_to_render_list(render_ops, render_basis, box.end - radius,                                           radius, color, thickness, 0b0100);
    add_circle_to_render_list(render_ops, render_basis, (V2){box.start.x + radius - one_pixel, box.end.y - radius}, radius, color, thickness, 0b0010);
    add_circle_to_render_list(render_ops, render_basis, box.start + radius - one_pixel,                             radius, color, thickness, 0b0001);

    add_box_to_render_list(render_ops, render_basis, (Rectangle){ (V2){ box.start.x+radius          , box.start.y                  }, (V2){ box.end.x-radius     , box.start.y+thickness } }, color);
    add_box_to_render_list(render_ops, render_basis, (Rectangle){ (V2){ box.start.x+radius-one_pixel, box.end.y-thickness          }, (V2){ box.end.x-radius     , box.end.y             } }, color);
    add_box_to_render_list(render_ops, render_basis, (Rectangle){ (V2){ box.start.x                 , box.start.y+radius           }, (V2){ box.start.x+thickness, box.end.y-radius      } }, color);
    add_box_to_render_list(render_ops, render_basis, (Rectangle){ (V2){ box.end.x-thickness         , box.start.y+radius-one_pixel }, (V2){ box.end.x            , box.end.y-radius      } }, color);
  }
  else
  {
    add_box_to_render_list(render_ops, render_basis, (Rectangle){ (V2){ box.start.x        , box.start.y           }, (V2){ box.end.x            , box.start.y+thickness } }, color);
    add_box_to_render_list(render_ops, render_basis, (Rectangle){ (V2){ box.start.x        , box.end.y-thickness   }, (V2){ box.end.x            , box.end.y             } }, color);
    add_box_to_render_list(render_ops, render_basis, (Rectangle){ (V2){ box.start.x        , box.start.y+thickness }, (V2){ box.start.x+thickness, box.end.y-thickness   } }, color);
    add_box_to_render_list(render_ops, render_basis, (Rectangle){ (V2){ box.end.x-thickness, box.start.y+thickness }, (V2){ box.end.x            , box.end.y-thickness   } }, color);
  }
}
