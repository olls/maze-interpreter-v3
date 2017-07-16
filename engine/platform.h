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


const u8  MAX_U8  = 255;
const u16 MAX_U16 = 65535;
const u32 MAX_U32 = 4294967295;
const u64 MAX_U64 = 18446744073709551615U;

const s8  MIN_S8  = -127;
const s8  MAX_S8  = 127;

const s16 MIN_S16 = -32767;
const s16 MAX_S16 = 32767;

const s32 MIN_S32 = -2147483647;
const s32 MAX_S32 = 2147483647;

const s64 MIN_S64 = -9223372036854775807;
const s64 MAX_S64 = 9223372036854775807;


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
