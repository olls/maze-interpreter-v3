union V2
{
  struct
  {
    float x;
    float y;
  };
};


struct Rectangle
{
  V2 start;
  V2 end;
};


union V3
{
  struct
  {
    float x;
    float y;
    float z;
  };
  struct
  {
    float r;
    float g;
    float b;
  };
};


union V4
{
  struct
  {
    float w;
    float x;
    float y;
    float z;
  };
  struct
  {
    float a;
    float r;
    float g;
    float b;
  };
};


union PixelColor
{
  struct
  {
    uint8_t b;
    uint8_t g;
    uint8_t r;
  };
  uint32_t pad;
};


// TODO: Replace use of directions with vectors?
enum Direction
{
  UP,
  DOWN,
  LEFT,
  RIGHT,
  STATIONARY
};
