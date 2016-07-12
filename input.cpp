void
process_input(Keys *keys, Input *input, u64 time_us)
{
  // NOTE: This functions purpose is to map the keyboard buttons to
  //         their meaning in the context of the game.

#define GET_INPUT_DOWN(inp, letter) inp = keys->alpha[letter - 'a'].down
#define GET_INPUT_ON_UP(inp, letter) inp = keys->alpha[letter - 'a'].on_up

  GET_INPUT_DOWN(input->step, 'j');
  GET_INPUT_ON_UP(input->step_mode_toggle, 'k');

  input->car_ticks_inc = keys->up.down;
  input->car_ticks_dec = keys->down.down;

  input->zoom_in = keys->equals.down;
  input->zoom_out = keys->minus.down;
}