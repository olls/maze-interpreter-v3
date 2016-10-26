void
setup_inputs(Keys *keys, Inputs *inputs)
{
  // TODO: Modifier keys

#define ALPHA_NUM_SYM(c) (keys->alpha_num_sym[(c)-MIN_CHAR])

  inputs->maps[RESTART].key_press = &(ALPHA_NUM_SYM('r').on_up);

  inputs->maps[RESET].key_press = &(ALPHA_NUM_SYM('n').on_up);

  inputs->maps[RELOAD].key_press = &(ALPHA_NUM_SYM('p').on_up);

  inputs->maps[SAVE].key_press = &(ALPHA_NUM_SYM('s').on_up);
  inputs->maps[SAVE].rate_limit = SLOW_KEY_REPEAT_RATE_LIMIT;

  inputs->maps[STEP].key_press = &(ALPHA_NUM_SYM('j').down);
  inputs->maps[STEP].rate_limit = SLOW_KEY_REPEAT_RATE_LIMIT;

  inputs->maps[STEP_MODE_TOGGLE].key_press = &(ALPHA_NUM_SYM('k').on_up);

  inputs->maps[ZOOM_IN].key_press = &(ALPHA_NUM_SYM('=').down);
  inputs->maps[ZOOM_IN].rate_limit = FAST_KEY_REPEAT_RATE_LIMIT;

  inputs->maps[ZOOM_OUT].key_press = &(ALPHA_NUM_SYM('-').down);
  inputs->maps[ZOOM_OUT].rate_limit = FAST_KEY_REPEAT_RATE_LIMIT;

  inputs->maps[SIM_TICKS_INC].key_press = &(keys->up.down);
  inputs->maps[SIM_TICKS_INC].rate_limit = FAST_KEY_REPEAT_RATE_LIMIT;

  inputs->maps[SIM_TICKS_DEC].key_press = &(keys->down.down);
  inputs->maps[SIM_TICKS_DEC].rate_limit = FAST_KEY_REPEAT_RATE_LIMIT;

  inputs->maps[CURSOR_LEFT].key_press = &(keys->left.down);
  inputs->maps[CURSOR_LEFT].rate_limit = SLOW_KEY_REPEAT_RATE_LIMIT;

  inputs->maps[CURSOR_RIGHT].key_press = &(keys->right.down);
  inputs->maps[CURSOR_RIGHT].rate_limit = SLOW_KEY_REPEAT_RATE_LIMIT;

  inputs->maps[CURSOR_BACKSPACE].key_press = &(keys->backspace.down);
  inputs->maps[CURSOR_BACKSPACE].rate_limit = SLOW_KEY_REPEAT_RATE_LIMIT;

  inputs->maps[LINE_BREAK].key_press = &(keys->enter.down);
  inputs->maps[LINE_BREAK].rate_limit = SLOW_KEY_REPEAT_RATE_LIMIT;

  for (u32 i = 0; i <= array_count(inputs->alpha_num_sym); ++i)
  {
    inputs->alpha_num_sym[i].key_press = &(keys->alpha_num_sym[i].down);
    inputs->alpha_num_sym[i].rate_limit = SLOW_KEY_REPEAT_RATE_LIMIT;
  }
}


void
update_input(Input *input, u64 time_us)
{
  if (input->rate_limit != 0)
  {
    if (*(input->key_press) && time_us >= input->last_active + (seconds_in_u(1) / input->rate_limit))
    {
      input->active = true;
      input->last_active = time_us;
    }
    else
    {
      input->active = false;
    }
  }
  else
  {
    input->active = *(input->key_press);
  }
}


void
update_inputs(Keys *keys, Inputs *inputs, u64 time_us)
{
  for (u32 i = 0; i < N_INPUTS; ++i)
  {
    Input *input = inputs->maps + i;
    update_input(input, time_us);
  }

  for (u32 i = 0; i <= array_count(inputs->alpha_num_sym); ++i)
  {
    Input *input = inputs->alpha_num_sym + i;
    update_input(input, time_us);
  }
}