enum CellDisplayType
{
  DISP_TYPE_ENCLOSED,
  DISP_TYPE_SINGLE,
  DISP_TYPE_L,
  DISP_TYPE_STRAIGHT,
  DISP_TYPE_T,
  DISP_TYPE_CROSS,

  N_DISP_TYPES
};


enum CellConnectedState
{
  WALKABLE,
  UNWALKABLE,
  UNCONNECTED
};


struct CellBitmaps
{
  Bitmap walkable[N_DISP_TYPES];
  Bitmap unwalkable[N_DISP_TYPES];
  Bitmap arrow;
  Bitmap splitter;
};


struct CellDisplay
{
  Bitmap *bitmap;
  Bitmap *bitmap_overlay;
  u32 rotate;
  V4 color;
};


const V2 CELL_VERTICES[] = {
  {0.3, 0.0},
  {0.7, 0.0},
  {0.7, 0.3},
  {1.0, 0.3},
  {1.0, 0.7},
  {0.7, 0.7},
  {0.7, 1.0},
  {0.3, 1.0},
  {0.3, 0.7},
  {0.0, 0.7},
  {0.0, 0.3},
  {0.3, 0.3}
};

const GLushort CELL_TRIANGLE_INDICES[] = {
  11, 0, 1, // Top
  11, 1, 2,
  2, 3, 4,  // Right
  2, 4, 5,
  5, 6, 7,  // Bottom
  5, 7, 8,
  8, 9, 10, // Left
  8, 10, 11,
  11, 2, 5, // Middle
  11, 5, 8
};


void
draw_cell(CellType type, V2 world_pos, u32 cell_radius, b32 hovered, CellBitmaps *cell_bitmaps, CellDisplay *cell_display = 0);
