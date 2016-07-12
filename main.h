#define DEBUG
// #define DEBUG_BLOCK_COLORS

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


#define kilobytes_to_bytes(n) (1024 * (n))
#define megabytes_to_bytes(n) (kilobytes_to_bytes(1024) * (n))
#define gigabytes_to_bytes(n) (megabytes_to_bytes(1024) * (n))

#define bytes_to_kilobytes(n) ((n) / 1024)
#define bytes_to_megabytes(n) ((n) / kilobytes_to_bytes(1024))
#define bytes_to_gigabytes(n) ((n) / bytes_to_megabytes(1024))

#define seconds_in_m(n) (1000 * (n))
#define seconds_in_u(n) (seconds_in_m(1000) * (n))

#define m_in_seconds(n) ((n) / (real32)seconds_in_m(1))
#define u_in_seconds(n) ((n) / (real32)seconds_in_u(1))

#define array_count(array) (sizeof(array) / sizeof((array)[0]))


const b32 FULLSCREEN = false;
const u32 WINDOW_WIDTH = 1920;
const u32 WINDOW_HEIGHT = 1080;

const u32 FPS = 60;
const u32 TOTAL_MEMORY = megabytes_to_bytes(50);


struct Key
{
  b32 down;
  b32 on_up;
  b32 on_down;
};

struct Keys
{
  b32 updated;

  Key minus;
  Key equals;
  Key space;
  Key up;
  Key down;
  Key left;
  Key right;
  Key alpha[26];
};


struct Mouse
{
  // TODO: Vector?
  u32 x;
  u32 y;

  b32 l_down;
  b32 r_down;

  V2 scroll;
};
