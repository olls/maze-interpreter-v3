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
  u32 x;
  u32 y;
  enum CellType type;
  union {
    u32 pause;
    u32 function_index;
  };
};


const u32 QUAD_STORE_N = 16;
struct QuadTree
{
  Rectangle bounds;

  u32 used;
  Cell cells[QUAD_STORE_N];

  QuadTree * top_right;
  QuadTree * top_left;
  QuadTree * bottom_right;
  QuadTree * bottom_left;
};


struct Maze
{
  // TODO: Not actually used ATM, get rid of?
  u32 width;
  u32 height;

  QuadTree tree;
  Function functions[MAX_FUNCTIONS];
};