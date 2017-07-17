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


bool
open_file(const char *filename, File *result, b32 write = false, s32 trunc_to = -1)
{
  b32 success = true;

  s32 open_flags;
  s32 mmap_flags;
  s32 mmap_prot;
  if (write)
  {
    open_flags = O_RDWR | O_TRUNC;
    mmap_prot = PROT_READ | PROT_WRITE;
    mmap_flags = MAP_SHARED;
  }
  else
  {
    open_flags = O_RDONLY;
    mmap_prot = PROT_READ;
    mmap_flags = MAP_PRIVATE;
  }

  result->fd = open(filename, open_flags);
  if (result->fd == -1)
  {
    printf("Failed to open file: \"%s\"\n", filename);
    success = false;
  }
  else
  {
    if (trunc_to < 0)
    {
      struct stat sb;
      if (fstat(result->fd, &sb) == -1)
      {
        printf("Failed to fstat : \"%s\"\n", filename);
        success = false;
      }
      else
      {
        result->size = sb.st_size;
      }
    }
    else
    {
      result->size = trunc_to;
      if (ftruncate(result->fd, result->size) == -1)
      {
        printf("Failed to ftruncate file: \"%s\"\n", filename);
        success = false;
      }
    }

    if (success)
    {
      result->text = (char *)mmap(NULL, result->size, mmap_prot, mmap_flags, result->fd, 0);
      if (result->text == MAP_FAILED)
      {
        printf("Failed to map file: \"%s\"\n", filename);
        success = false;
      }
      else
      {
        if (write)
        {
          result->read_only = NULL;
        }
        else
        {
          result->read_only = result->text;
        }
      }
    }
  }

  if (!success)
  {
    close(result->fd);
    result->fd = -1;
    result->text = 0;
    result->read_only = 0;
    result->size = 0;
  }

  return success;
}


b32
close_file(File *file, s32 trunc_to = -1)
{
  b32 error = false;

  if (munmap((void *)file->text, file->size) != 0)
  {
    printf("Error unmapping file.\n");
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
    printf("Error while closing file descriptor.\n");
    error = true;
  }

  return error;
}