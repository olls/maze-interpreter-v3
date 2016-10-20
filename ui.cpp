const V4 FRAME_COLOR = {1, 0.35, 0.5, .95};
const r32 FONT_SIZE = 0.2;
const r32 MENU_ANNIMATION_SPEED = .4;
V2 MENU_ITEM_SIZE = {250, 25};


Rectangle
get_memu_item_rect(Menu *menu, u32 n)
{
  Rectangle result;
  result.start = menu->pos + (V2){0, n}*MENU_ITEM_SIZE;
  result.end = result.start + MENU_ITEM_SIZE;
  return result;
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
ui_consume_mouse_clicks(GameState *game_state, RenderBasis *render_basis, UI *ui, Mouse *mouse)
{
  ui->mouse_click = false;

  if (ui->cell_type_menu.cell)
  {
    Rectangle rect;
    rect.start = ui->cell_type_menu.pos;
    rect.end = rect.start + MENU_ITEM_SIZE*(V2){1, ui->cell_type_menu.length};

    V2 world_mouse_coord = untransform_coord(render_basis, (V2){mouse->x, mouse->y});

    if (in_rectangle(world_mouse_coord, rect) && mouse->l_on_up && !game_state->currently_panning)
    {
      ui->mouse_click = true;
      mouse->l_on_up = false;
    }
  }
}


void
update_ui_menu(Menu *menu, V2 world_mouse_coord, b32 panning, b32 mouse_click, u64 time_us)
{
  menu->selected_selector.item_n = -1;

  s32 old_hover_selector_item_n = menu->hover_selector.item_n;
  menu->hover_selector.item_n = -1;

  if (menu->cell)
  {
    for (u32 item_index = 0;
         item_index < menu->length;
         ++item_index)
    {
      MenuItem *item = menu->items + item_index;

      Rectangle rect = get_memu_item_rect(menu, item_index);
      if (in_rectangle(world_mouse_coord, rect))
      {
        menu->hover_selector.item_n = item_index;
        if (old_hover_selector_item_n == -1)
        {
          menu->hover_selector.annimated_pos = rect.start;
        }

        if (mouse_click)
        {
          menu->cell->type = item->cell_type;
        }
      }

      if (menu->cell->type == item->cell_type)
      {
        menu->selected_selector.item_n = item_index;

        // Set pos directly if menu just opened, to avoid annimation from unshown position.
        if (menu->opened_on_frame == time_us)
        {
          V2 selected_item_pos = get_memu_item_rect(menu, item_index).start;
          menu->selected_selector.annimated_pos = selected_item_pos;
        }
      }
    }
  }
}


void
update_ui(GameState *game_state, RenderBasis *render_basis, UI *ui, Mouse *mouse, u64 time_us)
{
  V2 world_mouse_coord = untransform_coord(render_basis, (V2){mouse->x, mouse->y});

  update_ui_menu(&ui->cell_type_menu, world_mouse_coord, game_state->panning_this_frame, ui->mouse_click, time_us);
}


Rectangle
annimate_menu_item_fill(Menu *menu, MenuItemSelector *menu_item_selector)
{
  V2 target_pos = get_memu_item_rect(menu, menu_item_selector->item_n).start;
  V2 d_pos = target_pos - menu_item_selector->annimated_pos;

  menu_item_selector->annimated_pos += d_pos*MENU_ANNIMATION_SPEED;

  Rectangle result;
  result.start = menu_item_selector->annimated_pos;
  result.end = result.start + MENU_ITEM_SIZE;
  return result;
}


void
draw_ui_menu(RenderOperations *render_operations, RenderBasis *render_basis, Bitmaps *bitmaps, Menu *menu, u64 time_us)
{
  if (menu->cell)
  {
    for (u32 item_index = 0;
         item_index < menu->length;
         ++item_index)
    {
      MenuItem *item = menu->items + item_index;

      Rectangle menu_item_rect = get_memu_item_rect(menu, item_index);
      add_box_to_render_list(render_operations, render_basis, menu_item_rect, FRAME_COLOR);
    }

    if (menu->selected_selector.item_n >= 0)
    {
      Rectangle selected_rect = annimate_menu_item_fill(menu, &menu->selected_selector);
      V4 color = clamp(FRAME_COLOR + 0.5, 1);
      color.a = 0.7;
      add_box_to_render_list(render_operations, render_basis, selected_rect, color);
    }
    if (menu->hover_selector.item_n >= 0)
    {
      Rectangle hover_rect = annimate_menu_item_fill(menu, &menu->hover_selector);
      V4 color = clamp(FRAME_COLOR + (V4){0, 1, 0, 0}, 1);
      color.a = 0.7;
      add_box_to_render_list(render_operations, render_basis, hover_rect, color);
    }

    for (u32 item_index = 0;
         item_index < menu->length;
         ++item_index)
    {
      MenuItem *item = menu->items + item_index;

      Rectangle menu_item_rect = get_memu_item_rect(menu, item_index);
      draw_string(render_operations, render_basis, &bitmaps->font, menu_item_rect.start + 2, item->name, FONT_SIZE, (V4){1, 0, 0, 0});
    }
  }
}


void
draw_ui(RenderOperations *render_operations, RenderBasis *render_basis, Bitmaps *bitmaps, UI *ui, u64 time_us)
{
  draw_ui_menu(render_operations, render_basis, bitmaps, &ui->cell_type_menu, time_us);
}