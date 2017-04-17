// We want to be able to support:
// - grid of 2^32 by 2^32 cells
// - cell sub-position 0-1
// - rendering at 1080x1920:
//   - When rendering pick cell coordinate at centre of render
//     window as origin, and calculate all other coordinates
//     relative to it.
//   - This avoids precision problems:
//     - If you are zoomed all the way out you will be at the limit
//       of the precision with just the cell coordinates, but you
//       will not need any more precision for sub-cell positioning.
//     - If you are zoomed in at the very edge of the world, the
//       cell coordinates (at the limit of the precision) will be
//       re-centred around zero before adding the sub-cell
//       positioning to them, giving the sub-cell position nearly
//       the full precision.


struct WorldSpace
{
  u32 cell_x;
  u32 cell_y;

  // NOTE: Used for sub-cell positioning, 0-1.
  V2 offset;
};


struct RenderWindow
{
    WorldSpace render_origin;
    Rectangle window;
};