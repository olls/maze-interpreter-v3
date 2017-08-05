#define push_structs(memory, type, n) ((type *)push_mem(memory, (sizeof(type) * (n))))
#define push_struct(memory, type) ((type *)push_mem(memory, sizeof(type)))
void *
push_mem(Memory *memory, size_t bytes)
{
  void *result = memory->memory + memory->used;
  memory->used += bytes;

  if (memory->used >= memory->total)
  {
    printf("%ld\n", bytes);
    assert(!"Out of memory");
  }

  return result;
}


#define zero(mem, type) _zero((void *)(mem), sizeof(type))
#define zero_n(mem, type, n) _zero((void *)(mem), sizeof(type) * (n))
void
_zero(void *mem, u32 size)
{
  memset(mem, 0, size);
}


// To prevent u8 being passed to to_little_endian(u16 big)
s8
to_little_endian(s8 big)
{
  s8 result = big;
  return result;
}

u8
to_little_endian(u8 big)
{
  u8 result = big;
  return result;
}


u16
to_little_endian(u16 big)
{
  u8 result[2];
  u8 *bytes = (u8 *)&big;

  result[0] = bytes[1];
  result[1] = bytes[0];

  return *(u16 *)result;
}

s16
to_little_endian(s16 big)
{
  s8 result[2];
  s8 *bytes = (s8 *)&big;

  result[0] = bytes[1];
  result[1] = bytes[0];

  return *(s16 *)result;
}


u32
to_little_endian(u32 big)
{
  u8 result[4];
  u8 *bytes = (u8 *)&big;

  result[0] = bytes[3];
  result[1] = bytes[2];
  result[2] = bytes[1];
  result[3] = bytes[0];

  return *(u32 *)result;
}

s32
to_little_endian(s32 big)
{
  s8 result[4];
  s8 *bytes = (s8 *)&big;

  result[0] = bytes[3];
  result[1] = bytes[2];
  result[2] = bytes[1];
  result[3] = bytes[0];

  return *(s32 *)result;
}