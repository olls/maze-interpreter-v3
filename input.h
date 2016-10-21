const u32 FAST_KEY_REPEAT_RATE_LIMIT = 10;
const u32 SLOW_KEY_REPEAT_RATE_LIMIT = 5;


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
  SAVE,
  STEP,
  STEP_MODE_TOGGLE,
  ZOOM_IN,
  ZOOM_OUT,
  SIM_TICKS_INC,
  SIM_TICKS_DEC,

  N_INPUTS
};
