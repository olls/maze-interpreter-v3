Rectangle
get_menu_item_rect(Menu *menu, u32 n)
{
  Rectangle result;
  result.start = menu->pos + (V2){0, n}*MENU_ITEM_SIZE;
  result.end = result.start + (V2){menu->chars_wide*CHAR_SIZE.x*FONT_SIZE + MENU_ITEM_SIZE.y, MENU_ITEM_SIZE.y};
  return result;
}


Rectangle
get_button_rect(Button *button)
{
  Rectangle result;
  result.start = button->pos;
  result.end = result.start + (V2){button->length*CHAR_SIZE.x*FONT_SIZE, MENU_ITEM_SIZE.y};
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
    strncpy(item->name, CELL_TYPE_NAMES[item_index], MAX_UI_BOX_CHARS);
    item->cell_type = (CellType)item_index;

    u32 len;
    for (len = 0; item->name[len]; ++len);
    if (len > longest_menu_item)
    {
      longest_menu_item = len;
    }
  }

  ui->cell_type_menu.pos = (V2){10, 10};
  ui->cell_type_menu.chars_wide = longest_menu_item + 1;

  ui->test_input.length = 16;
  ui->test_input.allow_num = false;
  ui->test_input.allow_alpha = true;
  ui->test_input.allow_all = false;
}


void
ui_consume_mouse_clicks(GameState *game_state, RenderBasis *render_basis, UI *ui, Mouse *mouse, u64 time_us)
{
  b32 consume = false;

  V2 world_mouse_coord = untransform_coord(render_basis, (V2){mouse->x, mouse->y});

  if (ui->cell_type_menu.cell)
  {
    Rectangle rect;
    rect.start = ui->cell_type_menu.pos;
    rect.end = rect.start + MENU_ITEM_SIZE*(V2){1, ui->cell_type_menu.length};
    ui->cell_type_menu.clicked = false;

    if (in_rectangle(world_mouse_coord, rect) && !game_state->currently_panning)
    {
      ui->cell_type_menu.clicked = mouse->l_on_up;
      consume = true;
    }
  }

    {
    }
  }

  if (consume)
  {
    mouse->l_on_up = false;
  }
}


void
update_ui_menu(Menu *menu, V2 world_mouse_coord, u64 time_us)
{
  menu->selected_selector.item_n = -1;

  s32 old_hover_selector_item_n = menu->hover_selector.item_n;
  menu->hover_selector.item_n = -1;

  if (menu->cell)
  {
    b32 close_menu = false;

    for (u32 item_index = 0;
         item_index < menu->length;
         ++item_index)
    {
      MenuItem *item = menu->items + item_index;

      Rectangle rect = get_menu_item_rect(menu, item_index);
      if (in_rectangle(world_mouse_coord, rect))
      {
        menu->hover_selector.item_n = item_index;
        if (old_hover_selector_item_n == -1)
        {
          menu->hover_selector.annimated_pos = rect.start;
        }

        if (menu->clicked)
        {
          menu->cell->type = item->cell_type;
          close_menu = true;
        }
      }

      if (menu->cell->type == item->cell_type)
      {
        menu->selected_selector.item_n = item_index;

        // Set pos directly if menu just opened, to avoid annimation from unshown position.
        if (menu->opened_on_frame == time_us)
        {
          V2 selected_item_pos = get_menu_item_rect(menu, item_index).start;
          menu->selected_selector.annimated_pos = selected_item_pos;
        }
      }
    }

    if (close_menu)
    {
      menu->cell = 0;
    }
  }
}


void
update_text_input(InputBox *input_box, Inputs *inputs)
{
  if (inputs->maps[CURSOR_LEFT].active && input_box->cursor_pos > 0)
  {
    --input_box->cursor_pos;
  }
  if (inputs->maps[CURSOR_RIGHT].active && input_box->cursor_pos < input_box->length)
  {
    --input_box->cursor_pos;
  }
  if (inputs->maps[CURSOR_BACKSPACE].active)
  {
    if (input_box->cursor_pos > 0)
    {
      --input_box->cursor_pos;
    }
    if (input_box->cursor_pos < input_box->length)
    {
      input_box->text[input_box->cursor_pos] = '\0';
    }
  }

  for (u32 index = 0; index < array_count(inputs->alpha_num_sym); ++index)
  {
    char c = MIN_CHAR + index;
    if ((input_box->allow_all ||
         (input_box->allow_num   && c >= '0' & c <= '9') ||
         (input_box->allow_alpha && ((c >= 'a' & c <= 'z') || (c >= 'A' & c <= 'Z') || c == ' '))) &&
        inputs->alpha_num_sym[index].active &&
        input_box->cursor_pos < input_box->length)
    {
      input_box->text[input_box->cursor_pos++] = c;
      inputs->alpha_num_sym[index].active = false;
    }
  }
}


void
update_ui(GameState *game_state, RenderBasis *render_basis, UI *ui, Mouse *mouse, Inputs *inputs, u64 time_us)
{
  V2 world_mouse_coord = untransform_coord(render_basis, (V2){mouse->x, mouse->y});

  update_ui_menu(&ui->cell_type_menu, world_mouse_coord, time_us);

  update_text_input(&ui->test_input, inputs);
}


Rectangle
annimate_menu_item_fill(Menu *menu, MenuItemSelector *menu_item_selector)
{
  Rectangle menu_item_rect = get_menu_item_rect(menu, menu_item_selector->item_n);
  V2 d_pos = menu_item_rect.start - menu_item_selector->annimated_pos;

  menu_item_selector->annimated_pos += d_pos*MENU_ANNIMATION_SPEED;

  Rectangle result;
  result.start = menu_item_selector->annimated_pos;
  result.end = result.start + size(menu_item_rect);
  return result;
}


void
draw_ui_menu(RenderOperations *render_operations, RenderBasis *render_basis, Bitmap *font, Menu *menu, u64 time_us)
{
  if (menu->cell)
  {
    for (u32 item_index = 0;
         item_index < menu->length;
         ++item_index)
    {
      MenuItem *item = menu->items + item_index;

      Rectangle menu_item_rect = get_menu_item_rect(menu, item_index);
      add_box_to_render_list(render_operations, render_basis, menu_item_rect, FRAME_COLOR);

      u32 cell_radius = MENU_ITEM_SIZE.y*0.5;
      draw_cell(item->cell_type, render_operations, render_basis, (V2){menu_item_rect.end.x-cell_radius, menu_item_rect.start.y+cell_radius}, MENU_ITEM_SIZE.y*0.5, false);
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

      Rectangle menu_item_rect = get_menu_item_rect(menu, item_index);
      draw_string(render_operations, render_basis, font, menu_item_rect.start + 2, item->name, FONT_SIZE, (V4){1, 0, 0, 0});
    }
  }
}


void
draw_text_input(RenderOperations *render_operations, RenderBasis *render_basis, Bitmap *font, InputBox *input_box)
{
  Rectangle input_rect;
  input_rect.start = input_box->pos;
  input_rect.end = input_rect.start + (V2){input_box->length*CHAR_SIZE.x*FONT_SIZE, MENU_ITEM_SIZE.y};
  add_box_to_render_list(render_operations, render_basis, input_rect, FRAME_COLOR);

  draw_string(render_operations, render_basis, font, input_rect.start + 2, input_box->text, FONT_SIZE);
}


void
draw_button(RenderOperations *render_operations, RenderBasis *render_basis, Bitmap *font, Button *button, u64 time_us)
{
  Rectangle button_rect = get_button_rect(button);
  add_box_to_render_list(render_operations, render_basis, button_rect, FRAME_COLOR);

  draw_string(render_operations, render_basis, font, button_rect.start + 2, button->name, FONT_SIZE);

  if (button->hovered_at_time == time_us)
  {
    V4 color = clamp(FRAME_COLOR + (V4){0, 1, 0, 0}, 1);
    color.a = 0.7;
    add_box_to_render_list(render_operations, render_basis, button_rect, color);
  }
}


void
draw_car_input(RenderOperations *render_operations, RenderBasis *render_basis, Bitmap *font, CarInput *car_input, u64 time_us, V2 pos)
{
  car_input->input.pos = pos;
  car_input->done.pos = car_input->input.pos + (V2){0, MENU_ITEM_SIZE.y};

  draw_text_input(render_operations, render_basis, font, &car_input->input);
  draw_button(render_operations, render_basis, font, &car_input->done, time_us);
}


void
draw_ui(RenderOperations *render_operations, RenderBasis *render_basis, Bitmap *font, UI *ui, u64 time_us)
{
  draw_ui_menu(render_operations, render_basis, font, &ui->cell_type_menu, time_us);

  draw_text_input(render_operations, render_basis, font, &ui->test_input);
}