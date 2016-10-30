struct CellBitmaps
{
  Bitmap path_enclosed; // 0
  Bitmap path_single;   // 1
  Bitmap path_l;        // 2
  Bitmap path_straight; // 2
  Bitmap path_t;        // 3
  Bitmap path_cross;    // 4
};


V2
cell_coord_to_world(u32, u32, u32);


b32
draw_cell(CellType type, RenderOperations *render_operations, RenderBasis *render_basis, CellBitmaps *cell_bitmaps, V2 world_pos, u32 cell_radius, b32 hovered, Cell *neighbbours[4] = 0);

