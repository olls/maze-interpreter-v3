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
  CELL_PAUSE,

  N_CELL_TYPES
};

const char *CELL_TYPE_NAMES[] =
{
  "Null",                 // CELL_NULL
  "Start",                // CELL_START
  "Path",                 // CELL_PATH
  "Wall",                 // CELL_WALL
  "Hole",                 // CELL_HOLE
  "Splitter",             // CELL_SPLITTER
  "Function",             // CELL_FUNCTION
  "Once",                 // CELL_ONCE
  "Up Unless Detect",     // CELL_UP_UNLESS_DETECT
  "Down Unless Detect",   // CELL_DOWN_UNLESS_DETECT
  "Left Unless Detect",   // CELL_LEFT_UNLESS_DETECT
  "Right Unless Detect",  // CELL_RIGHT_UNLESS_DETECT
  "Inp",                  // CELL_INP
  "Out",                  // CELL_OUT
  "Up",                   // CELL_UP
  "Down",                 // CELL_DOWN
  "Left",                 // CELL_LEFT
  "Right",                // CELL_RIGHT
  "Pause"                 // CELL_PAUSE
};

const char *CELL_TYPE_TXT[] =
{
  "!!",  // CELL_NULL
  "^^",  // CELL_START
  "..",  // CELL_PATH
  "##",  // CELL_WALL
  "()",  // CELL_HOLE
  "<>",  // CELL_SPLITTER
  "AA",  // CELL_FUNCTION
  "--",  // CELL_ONCE
  "*U",  // CELL_UP_UNLESS_DETECT
  "*D",  // CELL_DOWN_UNLESS_DETECT
  "*L",  // CELL_LEFT_UNLESS_DETECT
  "*R",  // CELL_RIGHT_UNLESS_DETECT
  "<<",  // CELL_INP
  ">>",  // CELL_OUT
  "%U",  // CELL_UP
  "%D",  // CELL_DOWN
  "%L",  // CELL_LEFT
  "%R",  // CELL_RIGHT
  "00"   // CELL_PAUSE
};


struct Cell
{
  u32 x;
  u32 y;
  enum CellType type;
  char name[2];

  u64 hovered_at_time;

  u64 edit_mode_last_change;

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
  Cell *cache_hash[CELL_CACHE_SIZE];

  QuadTree tree;
};


V2
get_direction_cell_direction(CellType);