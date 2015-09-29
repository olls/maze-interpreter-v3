#define kilobytes(n) (1024 * n)
#define megabytes(n) (kilobytes(1024) * n)
#define gigabytes(n) (megabytes(1024) * n)

#define seconds_in_m(n) (1000 * n )
#define seconds_in_u(n) (seconds_in_m(1000) * n )

#define m_in_seconds(n) (n / (real32)seconds_in_m(1))
#define u_in_seconds(n) (n / (real32)seconds_in_u(1))

#define array_count(array) (sizeof(array) / sizeof((array)[0]))


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
  bool space_down;
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

// TODO: Sparce storage of cells and cars
struct Cell
{
  enum CellType type;
  int32_t data;
};

struct Car
{
  bool exists;
  int32_t value;

  // TODO: These are in world space, might need more range?
  float x;
  float y;
};

#define MAX_CARS (128)

struct Cars
{
  Car cars[MAX_CARS];
  uint32_t first_free;
};

void
init_car_mem(Cars * cars)
{
  cars->first_free = 0;
  for (uint32_t car_index = 0;
       car_index < MAX_CARS;
       ++car_index)
  {
    Car * car = cars->cars + car_index;
    car->exists = false;
    car->value = 1337;
  }
}
