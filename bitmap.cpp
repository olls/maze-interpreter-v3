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
    result->alpha_shift = get_first_bit_pos(file->alpha_mask);
  }
  else
  {
    result->red_shift = 16;
    result->green_shift = 8;
    result->blue_shift = 0;
  }

  result->pixels = (u8 *)file + file->bfOffBits;
  result->pixel_size = file->biBitCount / 8;
  result->file = file;

  log(L_Bitmap, "Loaded %s", filename);
}


void
blit_bitmap(FrameBuffer *frame_buffer, Bitmap *bitmap, V2 pos, V2 scale, V4 color_multiplier = (V4){1, 1, 1, 1}, r32 hue_shift = 0)
{
  // TODO: interpolation

  pos = round_down(pos);

  r32 width = (bitmap->file->biWidth - 1) * scale.x;
  r32 height = (bitmap->file->biHeight - 1) * scale.y;

  for (u32 dx = 0;
       dx < width;
       ++dx)
  {
    for (u32 dy = 0;
         dy < height;
         ++dy)
    {
      u32 pixel_x = pos.x + dx;
      u32 pixel_y = pos.y + dy;

      if ((pixel_x < frame_buffer->width) &&
          (pixel_y < frame_buffer->height))
      {
        r32 u = dx / scale.x;
        r32 v = bitmap->file->biHeight - 1 - (dy / scale.y);

        u32 bitmap_top_left_pos = (u32)(u)         + ((u32)(v) * bitmap->file->biWidth);
        u32 bitmap_top_right_pos = (u32)(u + 1)    + ((u32)(v) * bitmap->file->biWidth);
        u32 bitmap_bottom_left_pos = (u32)(u)      + ((u32)(v + 1) * bitmap->file->biWidth);
        u32 bitmap_bottom_right_pos = (u32)(u + 1) + ((u32)(v + 1) * bitmap->file->biWidth);

        u32 *bitmap_top_left = (u32 *)(bitmap->pixels + bitmap->pixel_size * bitmap_top_left_pos);
        u32 *bitmap_top_right = (u32 *)(bitmap->pixels + bitmap->pixel_size * bitmap_top_right_pos);
        u32 *bitmap_bottom_left = (u32 *)(bitmap->pixels + bitmap->pixel_size * bitmap_bottom_left_pos);
        u32 *bitmap_bottom_right = (u32 *)(bitmap->pixels + bitmap->pixel_size * bitmap_bottom_right_pos);

        V4 top_left_color = {(r32)((*bitmap_top_left >> bitmap->alpha_shift) & 0xff) / 255.0,
                             (r32)((*bitmap_top_left >> bitmap->red_shift) & 0xff) / 255.0,
                             (r32)((*bitmap_top_left >> bitmap->green_shift) & 0xff) / 255.0,
                             (r32)((*bitmap_top_left >> bitmap->blue_shift) & 0xff) / 255.0};
        V4 top_right_color = {(r32)((*bitmap_top_right >> bitmap->alpha_shift) & 0xff) / 255.0,
                              (r32)((*bitmap_top_right >> bitmap->red_shift) & 0xff) / 255.0,
                              (r32)((*bitmap_top_right >> bitmap->green_shift) & 0xff) / 255.0,
                              (r32)((*bitmap_top_right >> bitmap->blue_shift) & 0xff) / 255.0};
        V4 bottom_left_color = {(r32)((*bitmap_bottom_left >> bitmap->alpha_shift) & 0xff) / 255.0,
                                (r32)((*bitmap_bottom_left >> bitmap->red_shift) & 0xff) / 255.0,
                                (r32)((*bitmap_bottom_left >> bitmap->green_shift) & 0xff) / 255.0,
                                (r32)((*bitmap_bottom_left >> bitmap->blue_shift) & 0xff) / 255.0};
        V4 bottom_right_color = {(r32)((*bitmap_bottom_right >> bitmap->alpha_shift) & 0xff) / 255.0,
                                 (r32)((*bitmap_bottom_right >> bitmap->red_shift) & 0xff) / 255.0,
                                 (r32)((*bitmap_bottom_right >> bitmap->green_shift) & 0xff) / 255.0,
                                 (r32)((*bitmap_bottom_right >> bitmap->blue_shift) & 0xff) / 255.0};

        V4 color = lerp(lerp(top_left_color,    (u - (u32)u), top_right_color), (v - (u32)v),
                        lerp(bottom_left_color, (u - (u32)u), bottom_right_color));

        if (bitmap->file->biCompression != 3)
        {
          color.a = 1;
        }

        color *= color_multiplier;
        if (hue_shift)
        {
          color = shift_hue(color, hue_shift);
        }

        set_pixel(frame_buffer, pixel_x, pixel_y, color);
      }
    }
  }
}