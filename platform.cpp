#define push_structs(memory, type, n) ((type *)push_mem(memory, (sizeof(type) * (n))))
#define push_struct(memory, type) ((type *)push_mem(memory, sizeof(type)))
void *
push_mem(Memory *memory, size_t bytes)
{
  void *result = memory->memory + memory->used;
  memory->used += bytes;

  if (memory->used >= memory->total)
  {
    log(L_Main, "%ld\n", bytes);
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
