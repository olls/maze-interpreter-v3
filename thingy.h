
struct GameMemory
{
  uint32_t total;
  uint8_t * memory;
  uint8_t * pos;
};


struct Keys
{
  bool p_down;
  bool w_down;
  bool s_down;
  bool a_down;
  bool d_down;
};


struct Mouse
{
  uint32_t x;
  uint32_t y;

  bool l_down;
  bool r_down;
};

enum CellType
{
  CELL_NULL,
  CELL_START,
  CELL_PATH,
  CELL_WALL,
  CELL_HOLE,
  CELL_SPLITTER,
  CELL_FUNCTION,
  CELL_ONCE
};

struct Cell
{
  enum CellType type;
  int32_t data;
};
