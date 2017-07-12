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
  log(L_Bitmap, "Loading bitmap: \"%s\"", filename);

  u32 fd = open(filename, O_RDONLY);
  if (fd == -1)
  {
    printf("Failed to open \"%s\"\n", filename);
    assert(0);
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1)
  {
    printf("Failed to open \"%s\"\n", filename);
    assert(0);
  }

  Bitmap::BitmapFile *file = (Bitmap::BitmapFile *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if ((char *)file == MAP_FAILED)
  {
    printf("Failed to open \"%s\"\n", filename);
    assert(0);
  }

  log(L_Bitmap, "signature:                                         `%.2s`", file->signature);
  log(L_Bitmap, "file_size:                                         `%d`", file->file_size);
  log(L_Bitmap, "pixels_offset (Offset to start of Pixel Data):     `%d`", file->pixels_offset);
  log(L_Bitmap, "dib_header_size (Must be at least 40):             `%d`", file->dib_header_size);
  log(L_Bitmap, "width:                                             `%d`", file->width);
  log(L_Bitmap, "height:                                            `%d`", file->height);
  log(L_Bitmap, "bits_per_pixel (1, 4, 8, 16, 24, or 32):           `%d`", file->bits_per_pixel);
  log(L_Bitmap, "compression (0 = uncompressed):                    `%d`", file->compression);
  log(L_Bitmap, "image_size (may be zero for uncompressed images):  `%d`", file->image_size);
  log(L_Bitmap, "n_colors_table (Number of color table entries):    `%d`", file->n_colors_table);
  log(L_Bitmap, "red_mask:                                          `%8x`", file->red_mask);
  log(L_Bitmap, "green_mask:                                        `%8x`", file->green_mask);
  log(L_Bitmap, "blue_mask:                                         `%8x`", file->blue_mask);
  log(L_Bitmap, "alpha_mask:                                        `%8x`", file->alpha_mask);

  if (strncmp(file->signature, "BM", 2) != 0)
  {
    printf("File not a bitmap: \"%s\"\n", filename);
    assert(0);
  }

  if (file->bits_per_pixel != 8 && file->bits_per_pixel != 32)
  {
    printf("Bitmap bits_per_pixel not supported: \"%s\"\n", filename);
    assert(0);
  }

  if (file->compression == 3)
  {
    result->red_shift = get_first_bit_pos(file->red_mask);
    result->green_shift = get_first_bit_pos(file->green_mask);
    result->blue_shift = get_first_bit_pos(file->blue_mask);
    if (file->alpha_mask != 0)
    {
      result->alpha_shift = get_first_bit_pos(file->alpha_mask);
    }
    else
    {
      result->alpha_shift = -1;
    }
  }
  else
  {
    result->red_shift = 16;
    result->green_shift = 8;
    result->blue_shift = 0;
  }

  result->pixels = (u8 *)file + file->pixels_offset;
  result->file = file;

  log(L_Bitmap, "Loaded %s", filename);
}


vec4
get_bitmap_color(Bitmap *bitmap, u32 x, u32 y)
{
  vec4 result;

  if (x < bitmap->file->width &&
      y < bitmap->file->height)
  {
    u32 bytes_per_pixel = bitmap->file->bits_per_pixel / 8;
    u32 bytes_per_width = bitmap->file->width * bytes_per_pixel;

    // NOTE: Round up to 32 bit boundary
    if ((bytes_per_width % 4) != 0)
    {
      bytes_per_width += 4 - (bytes_per_width % 4);
    }

    u32 pixel_offset_bytes = (y * bytes_per_width) + (x * bytes_per_pixel);
    u32 raw_pixel = *(u32 *)(bitmap->pixels + pixel_offset_bytes);

    u32 pixel;
    if (bitmap->file->bits_per_pixel == 8)
    {
      u32 index = (u8)raw_pixel;
      pixel = *(&bitmap->file->color_table + index);
    }
    else
    {
      pixel = raw_pixel;
    }

    result = (vec4){(r32)((pixel >> bitmap->alpha_shift) & 0xff) / 255.0,
                    (r32)((pixel >> bitmap->red_shift) & 0xff) / 255.0,
                    (r32)((pixel >> bitmap->green_shift) & 0xff) / 255.0,
                    (r32)((pixel >> bitmap->blue_shift) & 0xff) / 255.0};

    if (bitmap->file->compression == 3 && bitmap->alpha_shift < 0)
    {
      result.a = 1;
    }

    // Basic alpha for 8 bit images
    // TODO: This isn't really a good solution
    if (bitmap->file->bits_per_pixel == 8)
    {
      if (result.r == 0xec && result.g == 0xd9 && result.b == 0x9f)
      {
        result = (vec4){0,1,1,1};
      }
      else
      {
        result.a = 1;
      }
    }
  }
  else
  {
    result = (vec4){1, 1, 0, 0};
  }

  return result;
}


#if 0
void
blit_bitmap(Renderer *renderer,
            RenderBasis *render_basis,
            Bitmap *bitmap,
            vec2 pos,
            BlitBitmapOptions *opts)
{
  if (opts->crop.start.x == -1)
  {
    opts->crop.start = (vec2){0, 0};
    opts->crop.end = (vec2){bitmap->file->width, bitmap->file->height};
  }

  r32 scale = render_basis->scale * opts->scale;

  vec2 crop_size = opts->crop.end - opts->crop.start;
  vec2 fract_pixel_space_size = crop_size * scale;

  Rectangle window_region = (Rectangle){(vec2){0, 0}, (vec2){renderer->frame_buffer.width, renderer->frame_buffer.height}};
  Rectangle render_region = render_basis->clip_region / render_basis->world_per_pixel;
  render_region = get_overlap(render_region, window_region);

  Rectangle fract_pixel_space;
  fract_pixel_space.start = transform_coord(render_basis, pos);
  fract_pixel_space.end = fract_pixel_space.start + fract_pixel_space_size;

  Rectangle cropped_fract_pixel_space = crop_to(fract_pixel_space, render_region);
  vec2 offset = fract_pixel_space.start - cropped_fract_pixel_space.start;

  Rectangle pixel_space = round_down(cropped_fract_pixel_space);
  vec2 fract_offset = pixel_space.start - cropped_fract_pixel_space.start;

  for (u32 pixel_y = pixel_space.start.y+1;
       pixel_y <= pixel_space.end.y;
       pixel_y++)
  {
    for (u32 pixel_x = pixel_space.start.x+1;
         pixel_x <= pixel_space.end.x;
         pixel_x++)
    {
      vec2 image_fract_pixel = ((vec2){(r32)pixel_x, (r32)pixel_y} - pixel_space.start) - offset + fract_offset;

      vec2 image_pos = image_fract_pixel / fract_pixel_space_size;

      vec2 rot_image_pos;
      if (opts->rotate == 90)
      {
        rot_image_pos.u =     image_pos.v;
        rot_image_pos.v =     image_pos.u;
      }
      else if (opts->rotate == 180)
      {
        rot_image_pos.u = 1 - image_pos.u;
        rot_image_pos.v =     image_pos.v;
      }
      else if (opts->rotate == 270)
      {
        rot_image_pos.u = 1 - image_pos.v;
        rot_image_pos.v = 1 - image_pos.u;
      }
      else
      {
        rot_image_pos.u =     image_pos.u;
        rot_image_pos.v = 1 - image_pos.v;
      }

      vec2 image_pixel = opts->crop.start + rot_image_pos * crop_size;

      // TODO: Fix interpolation on edges of crop

      vec4 top_left_color     = get_bitmap_color(bitmap, image_pixel.x,     image_pixel.y);
      vec4 top_right_color    = get_bitmap_color(bitmap, image_pixel.x + 1, image_pixel.y);
      vec4 bottom_left_color  = get_bitmap_color(bitmap, image_pixel.x,     image_pixel.y + 1);
      vec4 bottom_right_color = get_bitmap_color(bitmap, image_pixel.x + 1, image_pixel.y + 1);

      vec2 frac_pixel = image_pixel - round_down(image_pixel);

      vec4 color = top_left_color;

      if (opts->interpolation)
      {
        if (image_pixel.x < (bitmap->file->width - 1) &&
            image_pixel.y < (bitmap->file->height - 1))
        {
          color = lerp(lerp(top_left_color,    frac_pixel.x, top_right_color),
                       frac_pixel.y,
                       lerp(bottom_left_color, frac_pixel.x, bottom_right_color));
        }
        else if (image_pixel.x <  (bitmap->file->width - 1) &&
                 image_pixel.y >= (bitmap->file->height - 1))
        {
          color = lerp(top_left_color, frac_pixel.x, top_right_color);
        }
        else if (image_pixel.x >= (bitmap->file->width - 1) &&
                 image_pixel.y <  (bitmap->file->height - 1))
        {
          color = lerp(top_left_color, frac_pixel.y, bottom_left_color);
        }
      }

      color *= opts->color_multiplier;
      if (opts->invert)
      {
        color.r = 1 - color.r;
        color.g = 1 - color.g;
        color.b = 1 - color.b;
      }

      if (opts->hue_shift)
      {
        color = shift_hue(color, opts->hue_shift);
      }

      set_pixel(renderer, pixel_x, pixel_y, color);
    }
  }
}
#endif