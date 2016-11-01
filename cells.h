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


struct CellBitmaps
{
  Bitmap walkable[N_DISP_TYPES];
  Bitmap unwalkable[N_DISP_TYPES];
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
