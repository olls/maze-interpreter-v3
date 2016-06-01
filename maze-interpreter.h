// NOTE: 256 sub-pixel steps!
const u32 MIN_WORLD_PER_PIXEL = 256;
// const u32 MIN_WORLD_PER_PIXEL = 4096;
const u32 MAX_WORLD_PER_PIXEL = 65536;

// const char MAZE_FILENAME[] = "programs/test-huge.mz";
// const char MAZE_FILENAME[] = "programs/test-big.mz";
// const char MAZE_FILENAME[] = "programs/tree-big.mz";
// const char MAZE_FILENAME[] = "programs/non-square.mz";
const char MAZE_FILENAME[] = "test.mz";

const u32 CAR_CELL_PER_S = 20;


struct Car
{
  b32 update;
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


struct GameState
{
  b32 init;

  u32 world_per_pixel;

  // NOTE: Things are scaled relatively to cell_spacing.
  r32 zoom;
  u32 cell_spacing;
  r32 cell_margin;

  V2 last_mouse_pos;

  Maze maze;

  Cars cars;
  u64 last_car_update;
};