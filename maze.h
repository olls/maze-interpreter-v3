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
  CELL_UP_UNLESS_DETECT,
  CELL_DOWN_UNLESS_DETECT,
  CELL_LEFT_UNLESS_DETECT,
  CELL_RIGHT_UNLESS_DETECT,
  CELL_INP,
  CELL_OUT,
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
  char name[2];

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

  QuadTree *top_right;
  QuadTree *top_left;
  QuadTree *bottom_right;
  QuadTree *bottom_left;
};


const u32 CELL_CACHE_SIZE = 512;
struct Maze
{
  u32 width;
  u32 height;

  Cell *cache_hash[CELL_CACHE_SIZE];

  QuadTree tree;
  Function functions[MAX_FUNCTIONS];
};