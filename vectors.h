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


const V2 UP = {0, -1};
const V2 DOWN = {0, 1};
const V2 LEFT = {-1, 0};
const V2 RIGHT = {1, 0};
const V2 STATIONARY = {0, 0};
