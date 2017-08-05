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

const String CELL_TYPE_NAMES[] =
{
  String("Null"),                 // CELL_NULL
  String("Start"),                // CELL_START
  String("Path"),                 // CELL_PATH
  String("Wall"),                 // CELL_WALL
  String("Hole"),                 // CELL_HOLE
  String("Splitter"),             // CELL_SPLITTER
  String("Function"),             // CELL_FUNCTION
  String("Once"),                 // CELL_ONCE
  String("Up Unless Detect"),     // CELL_UP_UNLESS_DETECT
  String("Down Unless Detect"),   // CELL_DOWN_UNLESS_DETECT
  String("Left Unless Detect"),   // CELL_LEFT_UNLESS_DETECT
  String("Right Unless Detect"),  // CELL_RIGHT_UNLESS_DETECT
  String("Inp"),                  // CELL_INP
  String("Out"),                  // CELL_OUT
  String("Up"),                   // CELL_UP
  String("Down"),                 // CELL_DOWN
  String("Left"),                 // CELL_LEFT
  String("Right"),                // CELL_RIGHT
  String("Pause")                 // CELL_PAUSE
};

const u8 *CELL_TYPE_TXT[] =
{
  u8("!!"),  // CELL_NULL
  u8("^^"),  // CELL_START
  u8(".."),  // CELL_PATH
  u8("##"),  // CELL_WALL
  u8("()"),  // CELL_HOLE
  u8("<>"),  // CELL_SPLITTER
  u8("AA"),  // CELL_FUNCTION
  u8("--"),  // CELL_ONCE
  u8("*U"),  // CELL_UP_UNLESS_DETECT
  u8("*D"),  // CELL_DOWN_UNLESS_DETECT
  u8("*L"),  // CELL_LEFT_UNLESS_DETECT
  u8("*R"),  // CELL_RIGHT_UNLESS_DETECT
  u8("<<"),  // CELL_INP
  u8(">>"),  // CELL_OUT
  u8("%U"),  // CELL_UP
  u8("%D"),  // CELL_DOWN
  u8("%L"),  // CELL_LEFT
  u8("%R"),  // CELL_RIGHT
  u8("00")   // CELL_PAUSE
};


struct Cell
{
  u32 x;
  u32 y;
  enum CellType type;
  u8 name[2];

  u32 opengl_instance_position;

  u64 hovered_at_time;

  u64 edit_mode_last_change;

  union
  {
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


vec2
get_direction_cell_direction(CellType);