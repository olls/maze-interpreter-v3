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


#include <float.h>
#include <math.h>

#include "vectors.h"
#include "maths.h"
#include "blocks.h"


const uint32_t FPS = 60;
const uint32_t TOTAL_MEMORY = megabytes_to_bytes(50);

const uint32_t WINDOW_WIDTH = 1024;
const uint32_t WINDOW_HEIGHT = 600;

const uint32_t MAX_WORLD_PER_PIXEL = 65536;
const uint32_t MIN_WORLD_PER_PIXEL = 256;

// const char MAZE_FILENAME[] = "programs/test-huge.mz";
// const char MAZE_FILENAME[] = "programs/test-big.mz";
const char MAZE_FILENAME[] = "test.mz";


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
  // printf("Used %lumb\n", bytes_to_megabytes(game_memory->used));

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

  V2 scroll;
};


struct GameSetup
{
  uint32_t world_per_pixel;

  // NOTE: Things are scaled relatively to cell_spacing.
  uint32_t cell_spacing;
  float cell_margin;
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

  uint32_t cell_x;
  uint32_t cell_y;

  // NOTE: Used for sub-cell positioning, in world space.
  V2 offset;

  Direction direction;
};

#define MAX_CARS (10000)

struct Cars
{
  Car cars[MAX_CARS];
  uint32_t next_free;
};


void
printF(float f)
{
  printf("%f\n", f);
}

void
printU(uint32_t u)
{
  printf("%d\n", u);
}

void
printI(int32_t i)
{
  printf("%d\n", i);
}

void
printV(V4 vec)
{
  printf("(%f, %f, %f, %f)\n", vec.w, vec.x, vec.y, vec.z);
}

void
printV(V2 vec)
{
  printf("(%f, %f)\n", vec.x, vec.y);
}

void
printV(V3 vec)
{
  printf("(%f, %f, %f)\n", vec.x, vec.y, vec.z);
}

void
printR(Rectangle rect)
{
  printf("((%f, %f), (%f, %f))\n", rect.start.x, rect.start.y, rect.end.x, rect.end.y);
}