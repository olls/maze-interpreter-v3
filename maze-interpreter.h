#define kilobytes_to_bytes(n) (1024 * n)
#define megabytes_to_bytes(n) (kilobytes_to_bytes(1024) * n)
#define gigabytes_to_bytes(n) (megabytes_to_bytes(1024) * n)

#define bytes_to_kilobytes(n) (n / 1024)
#define bytes_to_megabytes(n) (n / kilobytes_to_bytes(1024))
#define bytes_to_gigabytes(n) (n / bytes_to_megabytes(1024))

#define seconds_in_m(n) (1000 * n )
#define seconds_in_u(n) (seconds_in_m(1000) * n )

#define m_in_seconds(n) (n / (real32)seconds_in_m(1))
#define u_in_seconds(n) (n / (real32)seconds_in_u(1))

#define array_count(array) (sizeof(array) / sizeof((array)[0]))

#define DEBUG

#ifdef DEBUG
#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)
#define assert(x) ((void)(!(x) && printf("Assertion Failed: "__FILE__":"S__LINE__":  ("#x")\n") && (exit(1), 1)))
#else
#define assert(x) ((void)sizeof(x))
#define printf(x, ...) ((void)sizeof(x))
#endif

#define invalid_code_path assert(!"Invalid Code Path! D:")


#include "blocks.h"


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


#define take_struct_mem(game_memory, struct_, num) {(struct_ *)take_mem(game_memory, (sizeof(struct_) * num))}
void *
take_mem(GameMemory * game_memory, size_t size)
{
  void * result = game_memory->memory + game_memory->used;
  game_memory->used += size;

  assert(game_memory->used < game_memory->total);

  return result;
}


struct FpsMeasurement
{
  uint64_t last_measure;
  uint32_t frame_count;
};


struct Keys
{
  bool space_down;
  bool up_down;
  bool down_down;
  bool left_down;
  bool right_down;
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


enum Direction
{
  UP,
  DOWN,
  LEFT,
  RIGHT,
  STATIONARY
};

Direction
reverse(Direction d)
{
  Direction result;

  switch (d)
  {
    case UP:
    {
      result = DOWN;
    } break;

    case DOWN:
    {
      result = UP;
    } break;

    case LEFT:
    {
      result = RIGHT;
    } break;

    case RIGHT:
    {
      result = LEFT;
    } break;

    default:
    {
      result = STATIONARY;
    } break;
  }

  return result;
}

struct Car
{
  bool update;
  int32_t value;

  // TODO: These are in world space, might need more range?
  //       1 byte per pixel + 1 byte per cell leaves 2 bytes (65536) for cell
  //       grid range.
  float x;
  float y;

  Direction direction;
};

#define MAX_CARS (10000)

struct Cars
{
  Car cars[MAX_CARS];
  uint32_t next_free;
};
