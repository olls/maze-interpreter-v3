union V2
{
  struct
  {
    r32 x;
    r32 y;
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
    r32 x;
    r32 y;
    r32 z;
  };
  struct
  {
    r32 r;
    r32 g;
    r32 b;
  };
};


union V4
{
  struct
  {
    r32 w;
    r32 x;
    r32 y;
    r32 z;
  };
  struct
  {
    r32 a;
    r32 r;
    r32 g;
    r32 b;
  };
};


union PixelColor
{
  struct
  {
    u8 b;
    u8 g;
    u8 r;
  };
  u32 pad;
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
