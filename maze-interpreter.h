// NOTE: 256 sub-pixel steps!
const u32 MIN_WORLD_PER_PIXEL = 256;
const u32 MAX_WORLD_PER_PIXEL = 65536;

const r32 MIN_ZOOM = 10;
const r32 MAX_ZOOM = 200;

// const char MAZE_FILENAME[] = "programs/test-huge.mz";
// const char MAZE_FILENAME[] = "programs/test-big.mz";
// const char MAZE_FILENAME[] = "programs/tree-big.mz";
// const char MAZE_FILENAME[] = "programs/non-square.mz";
const char MAZE_FILENAME[] = "test.mz";


struct GameState
{
  b32 init;

  u32 world_per_pixel;
  r32 d_zoom;
  r32 zoom;
  V2 scale_focus;

  RenderBasis last_frame_render_basis;

  // NOTE: Things are scaled relatively to cell_spacing.
  u32 cell_spacing;
  r32 cell_margin;

  V2 maze_pos;
  V2 last_mouse_pos;

  b32 single_step;
  u64 last_sim_tick;
  r32 sim_ticks_per_s;

  u32 sim_steps;

  Input inputs[N_INPUTS];
  Maze maze;
  Cars cars;
  Particles particles;

  Bitmap tile;
  Bitmap font;

  char persistent_str[256];
};