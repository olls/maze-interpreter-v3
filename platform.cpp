
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