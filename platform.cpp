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


void
open_file(const char *filename, File *result, u32 flags = O_RDONLY, s32 trunc_to = -1)
{
  result->fd = open(filename, flags);
  if (result->fd == -1)
  {
    printf("Failed to open file.\n");
    exit(1);
  }

  if (trunc_to < 0)
  {
    struct stat sb;
    if (fstat(result->fd, &sb) == -1)
    {
      printf("Failed to open file.\n");
      exit(1);
    }
    result->size = sb.st_size;
  }
  else
  {
    result->size = trunc_to;
    if (ftruncate(result->fd, result->size) == -1)
    {
      printf("Failed to open file.\n");
      exit(1);
    }
  }

  result->text = (char *)mmap(NULL, result->size, PROT_READ, MAP_PRIVATE, result->fd, 0);
  if (result->text == MAP_FAILED)
  {
    printf("Failed to open file.\n");
    exit(1);
  }
}


b32
close_file(File *file, s32 trunc_to = -1)
{
  b32 error = false;

  if (munmap((void *)file->text, file->size) != 0)
  {
    printf("Error unmapping file.");
    error = true;
  }

  if (trunc_to >= 0)
  {
    file->size = trunc_to;
    if (ftruncate(file->fd, file->size) == -1)
    {
      printf("Failed to truncate file for saving.\n");
    }
  }

  if (close(file->fd) != 0)
  {
    printf("Error while closing file descriptor.");
    error = true;
  }

  return error;
}