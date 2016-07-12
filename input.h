u32 key_repeat_rate_limit = 10;


struct Input
{
  b32 step;
  b32 step_mode_toggle;

  b32 zoom_in;
  b32 zoom_out;

  u64 last_car_ticks_inc;
  b32 car_ticks_inc;
  u64 last_car_ticks_dec;
  b32 car_ticks_dec;
};