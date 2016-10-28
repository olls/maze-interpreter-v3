Rectangle
get_menu_item_rect(Menu *menu, u32 n)
{
  Rectangle result;
  result.start = menu->pos + (V2){0, n}*MENU_ITEM_SIZE;
  result.end = result.start + (V2){menu->chars_wide*CHAR_SIZE.x*FONT_SIZE + MENU_ITEM_SIZE.y, MENU_ITEM_SIZE.y};
  return result;
}


Rectangle
get_input_box_rect(InputBox *input_box)
{
  Rectangle result;
  result.start = input_box->pos;
  result.end = result.start + (V2){input_box->length*CHAR_SIZE.x*FONT_SIZE, MENU_ITEM_SIZE.y};
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

  ui->car_inputs = 0;
}


void
ui_consume_mouse_clicks(GameState *game_state, RenderBasis *render_basis, UI *ui, Mouse *mouse, u64 time_us)
{
  b32 consume = false;

  V2 world_mouse_coord = untransform_coord(render_basis, (V2){mouse->x, mouse->y});

  if (ui->car_inputs)
  {
    CarInput *car_input = ui->car_inputs;
    while (car_input)
    {
      Button *button = &car_input->done;
      Rectangle button_rect = get_button_rect(button);
      if (in_rectangle(world_mouse_coord, button_rect))
      {
        button->hovered_at_time = time_us;
        button->activated = mouse->l_on_up;
        consume = true;
      }

      InputBox *input_box = &car_input->input;
      Rectangle input_box_rect = get_input_box_rect(input_box);
      if (in_rectangle(world_mouse_coord, input_box_rect) && mouse->l_on_up)
      {
        consume = true;
        input_box->active = mouse->l_on_up;
      }
      else if (mouse->l_on_up)
      {
        input_box->active = false;
      }

      car_input = car_input->next;
    }
  }

  ui->cell_type_menu.clicked = false;
  if (!consume && ui->cell_type_menu.cell)
  {
    Rectangle rect;
    rect.start = ui->cell_type_menu.pos;
    rect.end = rect.start + MENU_ITEM_SIZE*(V2){1, ui->cell_type_menu.length};

    if (in_rectangle(world_mouse_coord, rect))
    {
      ui->cell_type_menu.clicked = mouse->l_on_up;
      consume = true;
    }
  }

  if (consume)
  {
    mouse->l_on_up = false;
  }
}


void
open_cell_type_menu(GameState *game_state, Cell *cell_hovered_over, u64 time_us)
{
  // Only update time if the menu wasn't already open
  if (game_state->ui.cell_type_menu.cell == 0)
  {
    game_state->ui.cell_type_menu.opened_on_frame = time_us;
  }
  game_state->ui.cell_type_menu.cell = cell_hovered_over;
}


void
update_menu(Menu *menu, V2 world_mouse_coord, u64 time_us)
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
      menu->clicked = false;
    }
  }
}


b32
update_input_box(InputBox *input_box, Inputs *inputs)
{
  b32 result = false;

  if (input_box->active)
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
    if (inputs->maps[LINE_BREAK].active)
    {
      result = true;
    }

    if (input_box->allow_num && input_box->cursor_pos == 0 &&
        inputs->alpha_num_sym['-'-MIN_CHAR].active)
    {
      input_box->text[input_box->cursor_pos++] = '-';
      inputs->alpha_num_sym['-'-MIN_CHAR].active = false;
    }
    for (u32 index = 0; index < array_count(inputs->alpha_num_sym); ++index)
    {
      char c = MIN_CHAR + index;
      if ((input_box->allow_all ||
           (input_box->allow_num   && (c >= '0' && c <= '9')) ||
           (input_box->allow_alpha && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' '))) &&
          inputs->alpha_num_sym[index].active &&
          input_box->cursor_pos < input_box->length)
      {
        input_box->text[input_box->cursor_pos++] = c;
        inputs->alpha_num_sym[index].active = false;
      }
    }
  }

  return result;
}


void
update_button(Button *button, V2 world_mouse_coord)
{
}


void
update_car_inputs(GameState *game_state, UI *ui, V2 world_mouse_coord, Inputs *inputs)
{
  if (ui->car_inputs)
  {
    CarInput *car_input = ui->car_inputs;
    CarInput *prev_car_input = 0;
    while (car_input)
    {
      b32 enter_in_input = update_input_box(&car_input->input, inputs);
      update_button(&car_input->done, world_mouse_coord);

      if (car_input->done.activated || enter_in_input)
      {
        Car *car = get_car_with_id(&game_state->cars, car_input->car_id);
        assert(car);

        get_num(car_input->input.text, car_input->input.text+car_input->input.length, &car->value);

        if (prev_car_input)
        {
          prev_car_input->next = car_input->next;
        }
        else
        {
          ui->car_inputs = car_input->next;
        }

        CarInput *next = car_input->next;

        car_input->next = ui->free_car_inputs;
        ui->free_car_inputs = car_input;

        car_input = next;
      }
      else
      {
        prev_car_input = car_input;
        car_input = car_input->next;
      }
    }

    if (ui->car_inputs == 0)
    {
      game_state->finish_sim_step_move = true;
    }
  }
}


void
update_ui(GameState *game_state, RenderBasis *render_basis, UI *ui, Mouse *mouse, Inputs *inputs, u64 time_us)
{
  V2 world_mouse_coord = untransform_coord(render_basis, (V2){mouse->x, mouse->y});

  update_menu(&ui->cell_type_menu, world_mouse_coord, time_us);
  update_car_inputs(game_state, ui, world_mouse_coord, inputs);
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
draw_input_box(RenderOperations *render_operations, RenderBasis *render_basis, Bitmap *font, InputBox *input_box)
{
  Rectangle input_box_rect = get_input_box_rect(input_box);

  add_box_to_render_list(render_operations, render_basis, input_box_rect, FRAME_COLOR);

  if (input_box->active)
  {
    V4 cursor_color = {1, .9, .9, .9};
    Rectangle cursor_rect;
    cursor_rect.start = input_box_rect.start + (r32)input_box->cursor_pos*FONT_SIZE*CHAR_SIZE*(V2){1, 0};
    cursor_rect.end = cursor_rect.start + FONT_SIZE*CHAR_SIZE*(V2){0.15, 1};
    add_box_to_render_list(render_operations, render_basis, cursor_rect, cursor_color);
  }

  draw_string(render_operations, render_basis, font, input_box_rect.start + 2, input_box->text, FONT_SIZE);
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
draw_car_inputs(RenderOperations *render_operations, RenderBasis *render_basis, RenderBasis *world_render_basis, Bitmap *font, UI *ui, u64 time_us)
{
  Layouter layouter = {.next_free = 0};
  CarInput *car_input = ui->car_inputs;
  while (car_input)
  {
    V2 screen_car_pos = transform_coord(world_render_basis, car_input->car_world_pos);
    V2 ui_car_pos = untransform_coord(render_basis, screen_car_pos);

    Rectangle input_box_rect = get_input_box_rect(&car_input->input);

    add_box(&layouter, ui_car_pos, size(input_box_rect)*(V2){1, 2});
    car_input = car_input->next;
  }

  layout_boxes(&layouter, render_basis->clip_region);

  car_input = ui->car_inputs;
  u32 layouter_index = 0;
  while (car_input)
  {
    Rectangle *laidout_rect = layouter.rects + layouter_index++;

    V2 screen_car_pos = transform_coord(world_render_basis, car_input->car_world_pos);
    V2 ui_car_pos = untransform_coord(render_basis, screen_car_pos);

    car_input->input.pos = laidout_rect->start;
    car_input->done.pos = car_input->input.pos + (V2){0, MENU_ITEM_SIZE.y};

    draw_input_box(render_operations, render_basis, font, &car_input->input);
    draw_button(render_operations, render_basis, font, &car_input->done, time_us);

    add_line_to_render_list(render_operations, render_basis, laidout_rect->start, ui_car_pos, (V4){1, 0, 0.5, 0});

    car_input = car_input->next;
  }
}


void
draw_ui(RenderOperations *render_operations, RenderBasis *render_basis, RenderBasis *world_render_basis, Bitmap *font, UI *ui, u64 time_us)
{
  draw_ui_menu(render_operations, render_basis, font, &ui->cell_type_menu, time_us);
  draw_car_inputs(render_operations, render_basis, world_render_basis, font, ui, time_us);
}


void
init_car_input_box(Memory *memory, GameState *game_state, u32 car_id, s32 initial_value, u32 car_maze_x, u32 car_maze_y)
{
  UI *ui = &game_state->ui;

  CarInput *car_input = 0;

  if (ui->free_car_inputs)
  {
    car_input = ui->free_car_inputs;
    ui->free_car_inputs = car_input->next;
  }
  else
  {
    car_input = push_struct(memory, CarInput);
  }

  car_input->next = ui->car_inputs;
  ui->car_inputs = car_input;

  car_input->car_id = car_id;

  u32 car_radius = calc_car_radius(game_state->cell_spacing, game_state->cell_margin);
  car_input->car_world_pos = cell_coord_to_world(game_state->cell_spacing, car_maze_x, car_maze_y) + car_radius;

  zero(&car_input->input, InputBox);
  car_input->input.length = 10;
  car_input->input.allow_num = true;
  car_input->input.allow_alpha = false;
  car_input->input.allow_all = false;
  car_input->input.cursor_pos = fmted_str(car_input->input.text, car_input->input.length, "%d", initial_value);

  strcpy(car_input->done.name, "Set value");
  car_input->done.length = car_input->input.length;
  car_input->done.activated = false;
}


void
reset_car_inputs(UI *ui)
{
  if (ui->car_inputs)
  {
    CarInput *last_car_input = ui->car_inputs;
    while (last_car_input->next)
    {
      last_car_input = last_car_input->next;
    }

    last_car_input->next = ui->free_car_inputs;
    ui->free_car_inputs = ui->car_inputs;
    ui->car_inputs = 0;
  }
}