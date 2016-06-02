const u32 CAR_CELL_PER_S = 2;


struct Car
{
  b32 dead;

  s32 value;

  u32 cell_x;
  u32 cell_y;

  // NOTE: Used for sub-cell positioning, in world space.
  V2 offset;

  Direction direction;

  u32 pause_left;
  Direction unpause_direction;
};


// TODO: We probably want something better than this.
#define MAX_CARS (1000)
struct Cars
{
  Car cars[MAX_CARS];
  u32 next_free;
};
