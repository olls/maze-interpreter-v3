const uint32_t QUAD_STORE_N = 16;
struct QuadTree
{
  Rectangle bounds;

  uint32_t used;
  Cell cells[QUAD_STORE_N];

  QuadTree * top_right;
  QuadTree * top_left;
  QuadTree * bottom_right;
  QuadTree * bottom_left;
};


struct Maze
{
  uint32_t width;
  uint32_t height;

  QuadTree tree;
};