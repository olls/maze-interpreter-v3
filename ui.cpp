void
draw_ui_frame(FrameBuffer *frame_buffer, RenderBasis *render_basis, Rectangle frame)
{
  V4 frame_color = {1, 0.15, 0.3, 75};
  draw_thick_box_outline(frame_buffer, render_basis, frame, frame_color, 4);
}


void
draw_ui_menu(FrameBuffer *frame_buffer, RenderBasis *render_basis, Menu *menu)
{
  Rectangle rect;
  rect.start = render_basis->origin + menu->pos;
  rect.end = rect.start + (V2){70, 20 * menu->length};
  draw_ui_frame(frame_buffer, render_basis, rect);
}


void
draw_ui(FrameBuffer *frame_buffer, RenderBasis *render_basis, UI *ui)
{
  for (u32 menu_index = 0;
       menu_index < MAX_MENUS;
       ++menu_index)
  {
    Menu *menu = ui->menu_items + menu_index;
    if (menu->length)
    {
      draw_ui_menu(frame_buffer, render_basis, menu);
    }
  }
}


void
update_ui(UI *ui, Mouse *mouse)
{
  if (mouse->l_down)
  {
    ui->menu_items[0].length = N_CELL_TYPES;
    ui->menu_items[0].pos = (V2){0, 0};
  }
  else
  {
    ui->menu_items[0].length = 0;
  }
}