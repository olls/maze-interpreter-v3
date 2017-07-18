const r32 MIN_ZOOM = 10;
const r32 MAX_ZOOM = 200;

const r32 ZOOM_MIN_CELLS_ON_SCREEN = 100;
const r32 ZOOM_MAX_CELLS_ON_SCREEN = 2;

// const char MAZE_FILENAME[] = "programs/test-huge.mz";
// const char MAZE_FILENAME[] = "programs/test-big.mz";
// const char MAZE_FILENAME[] = "programs/tree-big.mz";
// const char MAZE_FILENAME[] = "programs/non-square.mz";
const char MAZE_FILENAME[] = "test.mz";


struct Bitmaps
{
  Bitmap tile;
  Bitmap font;
};


struct Panning
{
  r32 zoom_multiplier;
  r32 zoom;
  r32 old_zoom;
  vec2 scale_focus_pixels;

  WorldSpace world_maze_pos;
  vec2 last_mouse_pos;
  b32 currently_panning;
};


struct GameState
{
  b32 init;
  const char *filename;

  u32 world_per_pixel;

  Panning panning;

  CellInstancing cell_instancing;

  // NOTE: Things are scaled relatively to cell_spacing.
  r32 cell_margin;

  b32 single_step;
  u64 last_sim_tick;
  r32 sim_ticks_per_s;

  u32 sim_steps;
  b32 finish_sim_step_move;

  Inputs inputs;
  Maze maze;
  Functions functions;
  Cars cars;
  Particles particles;

  Bitmaps bitmaps;
  CellBitmaps cell_bitmaps;

  SVGOperation *arrow_svg;

  FT_Face font_face;

  char persistent_str[256];

  UI ui;
};