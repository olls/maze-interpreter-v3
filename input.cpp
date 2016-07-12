void
process_changed_input(Keys *keys, Input *input, u64 time_us)
{
  // NOTE: This functions purpose is to map the keyboard buttons to
  //         their meaning in the context of the game.

#define GET_INPUT_DOWN(inp, letter) inp = keys->alpha[letter - 'a'].down
#define GET_INPUT_ON_UP(inp, letter) inp = keys->alpha[letter - 'a'].on_up

  GET_INPUT_DOWN(input->step, 'j');
  GET_INPUT_ON_UP(input->step_mode_toggle, 'k');

  input->zoom_in = keys->equals.down;
  input->zoom_out = keys->minus.down;
}


void
update_input(Keys *keys, Input *input, u64 time_us)
{
  if (keys->up.down && time_us >= input->last_car_ticks_inc + (seconds_in_u(1) / key_repeat_rate_limit))
  {
    input->car_ticks_inc = true;
    input->last_car_ticks_inc = time_us;
  }
  else
  {
    input->car_ticks_inc = false;
  }

  if (keys->down.down && time_us >= input->last_car_ticks_dec + (seconds_in_u(1) / key_repeat_rate_limit))
  {
    input->car_ticks_dec = true;
    input->last_car_ticks_dec = time_us;
  }
  else
  {
    input->car_ticks_dec = false;
  }
}