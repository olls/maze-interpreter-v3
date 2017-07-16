struct vec2
{
  union
  {
    r32 elem[2];
    struct
    {
      r32 x;
      r32 y;
    };
  };
};


struct vec3
{
  union
  {
    r32 elem[3];
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
    struct
    {
      r32 h;
      r32 s;
      r32 v;
    };
    struct
    {
      vec2 xy;
      r32 _1;
    };
    struct
    {
      r32 _2;
      vec2 yz;
    };
  };
};


struct vec4
{
  union
  {
    r32 elem[4];
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
    struct
    {
      r32 _1;
      r32 h;
      r32 s;
      r32 v;
    };
    struct
    {
      vec2 wx;
      vec2 yz;
    };
    struct
    {
      r32 _2;
      vec2 xy;
      r32 _3;
    };
    struct
    {
      vec3 wxy;
      r32 _4;
    };
    struct
    {
      r32 _5;
      vec3 xyz;
    };
  };
};


struct Rectangle
{
  vec2 start;
  vec2 end;
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


const vec2 UP = {0, -1};
const vec2 DOWN = {0, 1};
const vec2 LEFT = {-1, 0};
const vec2 RIGHT = {1, 0};
const vec2 STATIONARY = {0, 0};


char
vector_to_compass_dir(vec2);