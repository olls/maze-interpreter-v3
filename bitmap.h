struct Bitmap
{
  struct BitmapFile {
    char signature[2];
    u32 file_size;
    u32 _;
    u32 pixels_offset;      // Offset to start of Pixel Data

    u32 dib_header_size;    // Must be at least 40
    u32 width;
    u32 height;
    u16 planes;             // Must be 1
    u16 bits_per_pixel;     // 1, 4, 8, 16, 24, or 32
    u32 compression;        // 0 = uncompressed
    u32 image_size;         // May be zero for uncompressed images
    u32 x_pixels_per_meter; // Preferred resolution
    u32 y_pixels_per_meter; // Preferred resolution
    u32 n_colors_table;     // Number of color table entries
    u32 important_colors;   // Number of significant colors

    u32 red_mask;
    u32 green_mask;
    u32 blue_mask;
    u32 alpha_mask;

    u32 color_space_type;
    u8 color_space_endpoints[36];

    u32 red_gamma;
    u32 green_gamma;
    u32 blue_gamma;

    u32 color_table;

  } __attribute__((packed)) *file;

  u8 *pixels;
  s32 alpha_shift;
  s32 red_shift;
  s32 green_shift;
  s32 blue_shift;
};


struct BlitBitmapOptions
{
  Rectangle crop;
  V4 color_multiplier;
  b32 invert;
  r32 hue_shift;
  b32 interpolation;
  r32 scale;
};
