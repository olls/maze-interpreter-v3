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
  CELL_UP,
  CELL_DOWN,
  CELL_LEFT,
  CELL_RIGHT,
  CELL_PAUSE
};

struct Cell
{
  enum CellType type;
  int32_t data;
};

const uint32_t BLOCK_NUM = 512;
const uint32_t BLOCK_SIDE_LENGTH = 64;
struct MazeBlock
{
  bool used;
  uint32_t x;
  uint32_t y;
  Cell cells[BLOCK_SIDE_LENGTH * BLOCK_SIDE_LENGTH];
  MazeBlock * next;
};

struct Maze
{
  uint32_t width;
  uint32_t height;

  MazeBlock hash[BLOCK_NUM];

#ifdef DEBUG
  uint32_t collisions;
#endif
};