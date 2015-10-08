
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
  CELL_RIGHT
};

// TODO: Sparse storage of cells and cars
struct Cell
{
  enum CellType type;
  int32_t data;
};

// TODO: Should probably be split spatially, to make it easier to index.
// NOTE: The cells should be stored contiguously, left to right, top to bottom.
const uint32_t BLOCK_NUM = 512;
const uint32_t BLOCK_SIDE_LENGTH = 64;
struct MazeBlock
{
  Cell cells[BLOCK_SIDE_LENGTH * BLOCK_SIDE_LENGTH];
};

struct Maze
{
  uint32_t width;
  uint32_t height;

  MazeBlock hash[BLOCK_NUM];
};