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
  uint32_t x;
  uint32_t y;
  enum CellType type;
  union {
    uint32_t pause;
    uint32_t function_index;
  } data;
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


enum FunctionType
{
  FunctionAssignment,
  FunctionIncrement,
  FunctionDecrement,
  FunctionConditional
};


const uint32_t MAX_FUNCTIONS = (26*2) * ((26*2) + 10); // [A-z][A-z0-9]
struct Function
{
  FunctionType type;
  char name[2];
};


struct Maze
{
  // TODO: Not actually used ATM, get rid of?
  uint32_t width;
  uint32_t height;

  QuadTree tree;
  Function functions[MAX_FUNCTIONS];
};