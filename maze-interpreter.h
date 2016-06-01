// NOTE: 256 sub-pixel steps!
const uint32_t MIN_WORLD_PER_PIXEL = 256;
// const uint32_t MIN_WORLD_PER_PIXEL = 4096;
const uint32_t MAX_WORLD_PER_PIXEL = 65536;

// const char MAZE_FILENAME[] = "programs/test-huge.mz";
// const char MAZE_FILENAME[] = "programs/test-big.mz";
// const char MAZE_FILENAME[] = "programs/tree-big.mz";
// const char MAZE_FILENAME[] = "programs/non-square.mz";
const char MAZE_FILENAME[] = "test.mz";

const uint32_t CAR_CELL_PER_S = 20;


struct Car
{
  bool update;
  bool dead;

  int32_t value;

  uint32_t cell_x;
  uint32_t cell_y;

  // NOTE: Used for sub-cell positioning, in world space.
  V2 offset;

  Direction direction;

  uint32_t pause_left;
  Direction unpause_direction;
};

// TODO: We probably want something better than this.
#define MAX_CARS (1000)
struct Cars
{
  Car cars[MAX_CARS];
  uint32_t next_free;
};


struct GameState
{
  bool init;

  uint32_t world_per_pixel;

  // NOTE: Things are scaled relatively to cell_spacing.
  float zoom;
  uint32_t cell_spacing;
  float cell_margin;

  V2 last_mouse_pos;

  Maze maze;

  Cars cars;
  uint64_t last_car_update;
};