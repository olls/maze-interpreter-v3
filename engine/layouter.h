const u32 MAX_PASSES = 8;


struct
Layouter
{
  Rectangle rects[128];
  u32 next_free;
};