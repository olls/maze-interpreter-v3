const V4 FRAME_COLOR = {1, 0.15, 0.3, 75};
const r32 FONT_SIZE = 0.2;
const r32 MENU_ANNIMATION_SPEED = 0.4;
V2 MENU_ITEM_SIZE = {250, 20};


Rectangle
get_memu_item_rect(Menu *menu, u32 n)
{
  Rectangle result;
  result.start = menu->pos + (V2){0, n}*MENU_ITEM_SIZE;
  result.end = result.start + MENU_ITEM_SIZE;
  return result;
}


void
draw_ui_menu(RenderOperations *render_operations, RenderBasis *render_basis, Bitmaps *bitmaps, Menu *menu, u64 time_us)
{
  Rectangle rect;
  rect.start = menu->pos;
  rect.end = rect.start + MENU_ITEM_SIZE * (V2){1, menu->length};
  rect = grow(rect, 2);

  draw_box_outline(render_operations, render_basis, rect, FRAME_COLOR, 1);

  for (u32 item_index = 0;
       item_index < menu->length;
       ++item_index)
  {
    MenuItem *item = menu->items + item_index;

    rect = get_memu_item_rect(menu, item_index);
    draw_box_outline(render_operations, render_basis, rect, FRAME_COLOR);
  }

  if (menu->annimated_hover >= 0)
  {
    Rectangle target_rect = get_memu_item_rect(menu, menu->annimated_hover);
    V2 d_pos = (target_rect.start - menu->annimated_hover_pos);
    menu->annimated_hover_pos += d_pos*MENU_ANNIMATION_SPEED;
    rect.start = menu->annimated_hover_pos;
    rect.end = rect.start + MENU_ITEM_SIZE;
    add_box_to_render_list(render_operations, render_basis, grow(rect, -1)+(Rectangle){(V2){0,0},(V2){0,1}}, clamp(FRAME_COLOR + (V4){0, 1, 0, 0}, 1));
  }
  if (menu->annimated_selected >= 0)
  {
    Rectangle target_rect = get_memu_item_rect(menu, menu->annimated_selected);
    V2 d_pos = (target_rect.start - menu->annimated_selected_pos);
    menu->annimated_selected_pos += d_pos*MENU_ANNIMATION_SPEED;
    rect.start = menu->annimated_selected_pos;
    rect.end = rect.start + MENU_ITEM_SIZE;
    add_box_to_render_list(render_operations, render_basis, grow(rect, -1)+(Rectangle){(V2){0,0},(V2){0,1}}, clamp(FRAME_COLOR + 0.5, 1));
  }

  for (u32 item_index = 0;
       item_index < menu->length;
       ++item_index)
  {
    MenuItem *item = menu->items + item_index;
    rect = get_memu_item_rect(menu, item_index);
    draw_string(render_operations, render_basis, &bitmaps->font, rect.start, item->name, FONT_SIZE, (V4){1, 0, 0, 0});
  }
}


void
init_ui(UI *ui)
{
  u32 longest_menu_item = 0;

  ui->cell_type_menu.length = N_CELL_TYPES;
  for (u32 item_index = 0;
       item_index < N_CELL_TYPES;
       ++item_index)
  {
    MenuItem *item = ui->cell_type_menu.items + item_index;
    strncpy(item->name, CELL_TYPE_NAMES[item_index], MAX_MENU_ITEM_NAME_LEN);
    item->cell_type = (CellType)item_index;

    u32 len;
    for (len = 0; item->name[len]; ++len);
    if (len > longest_menu_item)
    {
      longest_menu_item = len;
    }
  }
  ui->cell_type_menu.pos = (V2){10, 10};

  MENU_ITEM_SIZE.x = longest_menu_item * CHAR_SIZE.x * FONT_SIZE;
}


void
update_ui(GameState *game_state, RenderBasis *render_basis, UI *ui, Mouse *mouse, u64 time_us)
{
  ui->cell_type_menu.annimated_selected = -1;
  ui->cell_type_menu.annimated_hover = -1;

  for (u32 item_index = 0;
       item_index < ui->cell_type_menu.length;
       ++item_index)
  {
    MenuItem *item = ui->cell_type_menu.items + item_index;

    Rectangle rect = get_memu_item_rect(&ui->cell_type_menu, item_index);
    if (in_rectangle(untransform_coord(render_basis, (V2){mouse->x, mouse->y}), rect))
    {
      ui->cell_type_menu.annimated_hover = item_index;

      if (mouse->l_down && !game_state->panning_this_frame && ui->cell_currently_being_edited)
      {
        ui->cell_currently_being_edited->type = item->cell_type;
      }
    }

    if (ui->cell_currently_being_edited && ui->cell_currently_being_edited->type == item->cell_type)
    {
      ui->cell_type_menu.annimated_selected = item_index;
    }
  }
}


void
draw_ui(RenderOperations *render_operations, RenderBasis *render_basis, Bitmaps *bitmaps, UI *ui, u64 time_us)
{
  if (ui->cell_currently_being_edited)
  {
    draw_ui_menu(render_operations, render_basis, bitmaps, &ui->cell_type_menu, time_us);
  }
}