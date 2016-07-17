struct Bitmap
{
  struct BitmapFile {
    char bfType[2];       // The characters "BM"
    u32 bfSize;           // The size of the file in bytes
    u32 _;
    u32 bfOffBits;        // Offset to start of Pixel Data

    u32 biSize;           // Header Size - Must be at least 40
    u32 biWidth;          // Image width in pixels
    u32 biHeight;         // Image height in pixels
    u16 biPlanes;         // Must be 1
    u16 biBitCount;       // Bits per pixel - 1, 4, 8, 16, 24, or 32
    u32 biCompression;    // Compression type (0 = uncompressed)
    u32 biSizeImage;      // Image Size - may be zero for uncompressed images
    u32 biXPelsPerMeter;  // Preferred resolution in pixels per meter
    u32 biYPelsPerMeter;  // Preferred resolution in pixels per meter
    u32 biClrUsed;        // Number Color Map entries that are actually used
    u32 biClrImportant;   // Number of significant colors

    u32 red_mask;
    u32 green_mask;
    u32 blue_mask;
    u32 alpha_mask;
  } __attribute__((packed)) *file;

  u32 alpha_shift;
  u32 red_shift;
  u32 green_shift;
  u32 blue_shift;
};