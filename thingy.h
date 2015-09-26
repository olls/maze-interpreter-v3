

void
assert(bool cond)
{
  if (!cond)
  {
    printf("Assertion FAILED!! :(\n");
    exit(1);
  }
}


struct GameMemory
{
  size_t total;
  uint8_t * memory;
  size_t used;
};


void
init_mem(GameMemory * game_memory, size_t total)
{
  game_memory->total = total;
  game_memory->memory = (uint8_t *)malloc(total);
  game_memory->used = 0;

  assert(game_memory->memory != NULL);
}


#define take_sruct_mem(game_memory, size, obj_struct) take_mem(game_memory, ((size) * sizeof(obj_struct)))

void *
take_mem(GameMemory * game_memory, size_t size)
{
  void * result = game_memory->memory + game_memory->used;
  game_memory->used += size;

  assert(game_memory->used < game_memory->total);

  return result;
}


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

struct Car
{
  int32_t value;
};
