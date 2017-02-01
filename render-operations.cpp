void
add_to_render_list(RenderOperations *render_ops, RenderOperation *operation)
{
  if (render_ops->next_free >= MAX_RENDER_OPS)
  {
    log(L_Render, "Error: Ran out of render operation slots!");
  }
  else
  {
    RenderOperation *render_op_slot = render_ops->ops + render_ops->next_free++;
    *render_op_slot = *operation;
  }
}


void
add_circle_to_render_list(RenderOperations *render_ops, RenderBasis *render_basis, V2 world_pos, r32 world_radius, V4 color, r32 world_outline_width = -1)
{
  RenderOperation operation;
  operation.type = RENDER_OP_CIRCLE;
  operation.render_basis = *render_basis;

  operation.circle.world_pos = world_pos;
  operation.circle.world_radius = world_radius;
  operation.circle.color = color;
  operation.circle.world_outline_width = world_outline_width;

  add_to_render_list(render_ops, &operation);
}


void
add_box_to_render_list(RenderOperations *render_ops, RenderBasis *render_basis, Rectangle box, V4 color)
{
  RenderOperation operation;
  operation.type = RENDER_OP_BOX;
  operation.render_basis = *render_basis;

  operation.box.box = box;
  operation.box.color = color;

  add_to_render_list(render_ops, &operation);
}


void
add_fast_box_to_render_list(RenderOperations *render_ops, RenderBasis *render_basis, Rectangle box, PixelColor color)
{
  RenderOperation operation;
  operation.type = RENDER_OP_FAST_BOX;
  operation.render_basis = *render_basis;

  operation.fast_box.box = box;
  operation.fast_box.color = color;

  add_to_render_list(render_ops, &operation);
}


void
add_line_to_render_list(RenderOperations *render_ops, RenderBasis *render_basis, V2 world_start, V2 world_end, V4 color, r32 width = 50)
{
  RenderOperation operation;
  operation.type = RENDER_OP_LINE;
  operation.render_basis = *render_basis;

  operation.line.world_start = world_start;
  operation.line.world_end = world_end;
  operation.line.color = color;
  operation.line.width = width;

  add_to_render_list(render_ops, &operation);
}


void
add_bitmap_to_render_list(RenderOperations *render_ops, RenderBasis *render_basis, Bitmap *bitmap, V2 pos, BlitBitmapOptions *opts = 0)
{
  RenderOperation operation;
  operation.type = RENDER_OP_BITMAP;
  operation.render_basis = *render_basis;

  operation.bitmap.bitmap = bitmap;
  operation.bitmap.pos = pos;

  if (opts == 0)
  {
    get_default_blit_bitmap_options(&operation.bitmap.opts);
  }
  else
  {
    operation.bitmap.opts = *opts;
  }

  add_to_render_list(render_ops, &operation);
}


void
consume_render_operations(Renderer *renderer, RenderOperations *render_ops, Rectangle clip_region)
{
  u32 next_to_consume = 0;
  while (next_to_consume < render_ops->next_free)
  {
    RenderOperation *operation = render_ops->ops + next_to_consume++;

    RenderBasis render_basis = operation->render_basis;
    render_basis.clip_region = clip_region * render_basis.world_per_pixel;

    switch (operation->type)
    {
      case (RENDER_OP_CIRCLE):
      {
        render_circle(renderer, &render_basis, operation->circle.world_pos, operation->circle.world_radius, operation->circle.color, operation->circle.world_outline_width);
      } break;
      case (RENDER_OP_BOX):
      {
        render_box(renderer, &render_basis, operation->box.box, operation->box.color);
      } break;
      case (RENDER_OP_FAST_BOX):
      {
        fast_render_box(renderer, &render_basis, operation->fast_box.box, operation->fast_box.color);
      } break;
      case (RENDER_OP_LINE):
      {
        render_line(renderer, &render_basis, operation->line.world_start, operation->line.world_end, operation->line.color, operation->line.width);
      } break;
      case (RENDER_OP_BITMAP):
      {
        blit_bitmap(renderer, &render_basis, operation->bitmap.bitmap, operation->bitmap.pos, &operation->bitmap.opts);
      } break;
    };
  }
}