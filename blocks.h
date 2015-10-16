enum CellType
{
  CELL_NULL,
  CELL_START,
  CELL_PATH,
  CELL_WALL,
  CELL_HOLE,
  CELL_SPLITTER,
  CELL_FUNCTION,
  CELL_ONCE,
  CELL_SIGNAL,
  CELL_INC,
  CELL_DEC,
  CELL_UP,
  CELL_DOWN,
  CELL_LEFT,
  CELL_RIGHT,
  CELL_PAUSE
};

struct Cell
{
  // TODO: It's a shame these are stored here, maybe store the cells
  //       within a quad-tree block in a grid?
  uint32_t x;
  uint32_t y;
  enum CellType type;
  int32_t data;
};

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

#ifdef DEBUG
  uint32_t subdivisions;
#endif
};