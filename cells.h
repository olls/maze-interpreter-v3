struct CellBitmaps
{
  Bitmap path_enclosed; // 0
  Bitmap path_single;   // 1
  Bitmap path_l;        // 2
  Bitmap path_straight; // 2
  Bitmap path_t;        // 3
  Bitmap path_cross;    // 4
  Bitmap unwalkable;
};


struct CellDisplay
{
  Bitmap *bitmap;
  u32 rotate;
};


V2
cell_coord_to_world(u32, u32, u32);


void
draw_cell(RenderOperations *render_operations, RenderBasis *render_basis, CellType type, V2 world_pos, u32 cell_radius, b32 hovered, CellBitmaps *cell_bitmaps, CellDisplay *cell_display = 0);
