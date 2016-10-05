u32 fast_key_repeat_rate_limit = 10;
u32 slow_key_repeat_rate_limit = 5;


struct Input
{
  b32 active;
  b32 *key_press;
  u32 rate_limit;
  u64 last_active;
};


enum Inputs
{
  RESTART,
  RESET,
  RELOAD,
  STEP,
  STEP_MODE_TOGGLE,
  ZOOM_IN,
  ZOOM_OUT,
  SIM_TICKS_INC,
  SIM_TICKS_DEC,

  N_INPUTS
};
