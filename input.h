u32 key_repeat_rate_limit = 10;


struct Input
{
  b32 active;
  b32 *key_press;
  u32 rate_limit;
  u64 last_active;
};


enum Inputs
{
  STEP,
  STEP_MODE_TOGGLE,
  ZOOM_IN,
  ZOOM_OUT,
  CAR_TICKS_INC,
  CAR_TICKS_DEC,

  N_INPUTS
};
