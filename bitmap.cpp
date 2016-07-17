#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


u32
get_first_bit_pos(u32 x)
{
  u32 result = 0;
  while ((x & 0x1) == 0)
  {
    x >>= 1;
    ++result;
  }
  return result;
}


void
load_bitmap(Bitmap *result, const char *filename)
{
  u32 fd = open(filename, O_RDONLY);
  if (fd == -1)
  {
    log(L_Bitmap, "Failed to open %s", filename);
    assert(0);
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1)
  {
    log(L_Bitmap, "Failed to open %s", filename);
    assert(0);
  }

  Bitmap::BitmapFile *file = (Bitmap::BitmapFile *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if ((char *)file == MAP_FAILED)
  {
    log(L_Bitmap, "Failed to open %s", filename);
    assert(0);
  }

  log(L_Bitmap, "char bfType[2] (The characters \"BM\")                               : `%.2s`", file->bfType);
  log(L_Bitmap, "u32 bfSize (The size of the file in bytes)                         : `%d`", file->bfSize);
  log(L_Bitmap, "u32 bfOffBits (Offset to start of Pixel Data)                      : `%d`", file->bfOffBits);
  log(L_Bitmap, "u32 biSize (Header Size - Must be at least 40)                     : `%d`", file->biSize);
  log(L_Bitmap, "u32 biWidth (Image width in pixels)                                : `%d`", file->biWidth);
  log(L_Bitmap, "u32 biHeight (Image height in pixels)                              : `%d`", file->biHeight);
  log(L_Bitmap, "u16 biBitCount (Bits per pixel - 1, 4, 8, 16, 24, or 32)           : `%d`", file->biBitCount);
  log(L_Bitmap, "u32 biCompression (Compression type (0 = uncompressed))            : `%d`", file->biCompression);
  log(L_Bitmap, "u32 biSizeImage (Image Size - may be zero for uncompressed images) : `%d`", file->biSizeImage);
  log(L_Bitmap, "u32 red_mask    : `%8x`", file->red_mask);
  log(L_Bitmap, "u32 green_mask  : `%8x`", file->green_mask);
  log(L_Bitmap, "u32 blue_mask   : `%8x`", file->blue_mask);

  assert(strcmp((char *)(&file->bfType), "BM"));

  if (file->biCompression == 3)
  {
    result->red_shift = get_first_bit_pos(file->red_mask);
    result->green_shift = get_first_bit_pos(file->green_mask);
    result->blue_shift = get_first_bit_pos(file->blue_mask);
    result->alpha_shift = get_first_bit_pos(!(result->red_shift & result->green_shift & result->blue_shift));
  }
  else
  {
    result->red_shift = 16;
    result->green_shift = 8;
    result->blue_shift = 0;
  }

  result->file = file;

  log(L_Bitmap, "Loaded %s", filename);
}


void
blit_bitmap(FrameBuffer *frame_buffer, Bitmap *bitmap, V2 pos)
{
  pos = round_down(pos);

  for (u32 u = 0;
       u < bitmap->file->biWidth;
       ++u)
  {
    for (u32 v = 0;
         v < bitmap->file->biHeight;
         ++v)
    {
      u32 pixel_x = pos.x + u;
      u32 pixel_y = (pos.y + bitmap->file->biHeight - 1) - v;

      if ((pixel_x < frame_buffer->width) &&
          (pixel_y < frame_buffer->height))
      {
        u32 *bitmap_color = (u32 *)((u8 *)bitmap->file + bitmap->file->bfOffBits + (u + (v * bitmap->file->biWidth)) * (bitmap->file->biBitCount / 8));
        V4 color = {(r32)((*bitmap_color >> bitmap->alpha_shift) & 0xff) / 255.0,
                    (r32)((*bitmap_color >> bitmap->red_shift) & 0xff) / 255.0,
                    (r32)((*bitmap_color >> bitmap->green_shift) & 0xff) / 255.0,
                    (r32)((*bitmap_color >> bitmap->blue_shift) & 0xff) / 255.0};

        if (bitmap->file->biCompression != 3)
        {
          color.a = 1;
        }

        set_pixel(frame_buffer, pixel_x, pixel_y, color);
      }
    }
  }
}