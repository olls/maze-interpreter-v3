void
setup_inputs(Keys *keys, Input inputs[])
{
  // TODO: Modifier keys
#define GET_ALPHA_KEY(letter) (keys->alpha + (letter - 'a'))

  inputs[RESTART].key_press = &(GET_ALPHA_KEY('r')->on_up);

  inputs[RESET].key_press = &(GET_ALPHA_KEY('n')->on_up);

  inputs[RELOAD].key_press = &(GET_ALPHA_KEY('p')->on_up);

  inputs[SAVE].key_press = &(GET_ALPHA_KEY('s')->on_up);
  inputs[SAVE].rate_limit = SLOW_KEY_REPEAT_RATE_LIMIT;

  inputs[STEP].key_press = &(GET_ALPHA_KEY('j')->down);
  inputs[STEP].rate_limit = SLOW_KEY_REPEAT_RATE_LIMIT;

  inputs[STEP_MODE_TOGGLE].key_press = &(GET_ALPHA_KEY('k')->on_up);

  inputs[ZOOM_IN].key_press = &(keys->equals.down);
  inputs[ZOOM_IN].rate_limit = FAST_KEY_REPEAT_RATE_LIMIT;

  inputs[ZOOM_OUT].key_press = &(keys->minus.down);
  inputs[ZOOM_OUT].rate_limit = FAST_KEY_REPEAT_RATE_LIMIT;

  inputs[SIM_TICKS_INC].key_press = &(keys->up.down);
  inputs[SIM_TICKS_INC].rate_limit = FAST_KEY_REPEAT_RATE_LIMIT;

  inputs[SIM_TICKS_DEC].key_press = &(keys->down.down);
  inputs[SIM_TICKS_DEC].rate_limit = FAST_KEY_REPEAT_RATE_LIMIT;
}


void
update_inputs(Keys *keys, Input inputs[], u64 time_us)
{
  for (u32 i = 0; i < N_INPUTS; ++i)
  {
    Input *input = inputs + i;

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
}