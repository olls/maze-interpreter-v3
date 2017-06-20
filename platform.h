typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float r32;
typedef double r64;
typedef uint32_t b32;


struct Memory
{
  u8 * memory;
  size_t used;
  size_t total;
};


struct File
{
  s32 fd;
  char *text;
  const char *read_only;
  s32 size;
};
