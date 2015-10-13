union V2
{
  struct
  {
    float x;
    float y;
  };
};


V2
operator*(float c, V2 vec)
{
  V2 result;
  result.x = c * vec.x;
  result.y = c * vec.y;

  return result;
}
V2
operator*(V2 vec, float c)
{
  V2 result = c * vec;
  return result;
}
V2 &
operator*=(V2 & vec, float c)
{
  vec = vec * c;
  return vec;
}


V2
operator/(float c, V2 vec)
{
  V2 result;
  result.x = c / vec.x;
  result.y = c / vec.y;

  return result;
}
V2
operator/(V2 vec, float c)
{
  V2 result;
  result.x = vec.x / c;
  result.y = vec.y / c;

  return result;
}
V2 &
operator/=(V2 & vec, float c)
{
  vec = vec / c;
  return vec;
}


V2
operator+(float c, V2 vec)
{
  V2 result;
  result.x = c + vec.x;
  result.y = c + vec.y;

  return result;
}
V2
operator+(V2 vec, float c)
{
  V2 result = c + vec;
  return result;
}
V2 &
operator+=(V2 & vec, float c)
{
  vec = vec + c;
  return vec;
}

V2
operator+(V2 a, V2 b)
{
  V2 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;

  return result;
}
V2 &
operator+=(V2 & a, V2 b)
{
  a = a + b;
  return a;
}


V2
operator-(V2 vec)
{
  V2 result;
  result.x = -vec.x;
  result.y = -vec.y;

  return result;
}

V2
operator-(float c, V2 vec)
{
  V2 result;
  result.x = c - vec.x;
  result.y = c - vec.y;

  return result;
}
V2
operator-(V2 vec, float c)
{
  V2 result;
  result.x = vec.x - c;
  result.y = vec.y - c;

  return result;
}
V2 &
operator-=(V2 & vec, float c)
{
  vec = vec - c;
  return vec;
}

V2
operator-(V2 a, V2 b)
{
  V2 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;

  return result;
}
V2 &
operator-=(V2 & a, V2 b)
{
  a = a - b;
  return a;
}


V2
min_V2(V2 a, V2 b)
{
  V2 result;
  result.x = fmin(a.x, b.x);
  result.y = fmin(a.y, b.y);
  return result;
}

V2
max_V2(V2 a, V2 b)
{
  V2 result;
  result.x = fmax(a.x, b.x);
  result.y = fmax(a.y, b.y);
  return result;
}


struct Rectangle
{
  V2 start;
  V2 end;
};

bool
in_rectangle(V2 test, Rectangle rect)
{
  bool result = ((test.x >= rect.start.x) &&
                 (test.x < rect.end.x) &&
                 (test.y >= rect.start.y) &&
                 (test.y < rect.end.y));
  return result;
}

Rectangle
crop_to(Rectangle a, Rectangle bound)
{
  Rectangle result;
  result.start = max_V2(a.start, bound.start);
  result.end   = min_V2(a.end,   bound.end);
  return result;
}


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


V3
operator*(float c, V3 vec)
{
  V3 result;
  result.x = c * vec.x;
  result.y = c * vec.y;
  result.z = c * vec.z;

  return result;
}
V3
operator*(V3 vec, float c)
{
  V3 result = c * vec;
  return result;
}
V3 &
operator*=(V3 & vec, float c)
{
  vec = vec * c;
  return vec;
}


V3
operator/(float c, V3 vec)
{
  V3 result;
  result.x = c / vec.x;
  result.y = c / vec.y;
  result.z = c / vec.z;

  return result;
}
V3
operator/(V3 vec, float c)
{
  V3 result;
  result.x = vec.x / c;
  result.y = vec.y / c;
  result.z = vec.z / c;

  return result;
}
V3 &
operator/=(V3 & vec, float c)
{
  vec = vec / c;
  return vec;
}


V3
operator+(float c, V3 vec)
{
  V3 result;
  result.x = c + vec.x;
  result.y = c + vec.y;
  result.z = c + vec.z;

  return result;
}
V3
operator+(V3 vec, float c)
{
  V3 result = c + vec;
  return result;
}
V3 &
operator+=(V3 & vec, float c)
{
  vec = vec + c;
  return vec;
}

V3
operator+(V3 a, V3 b)
{
  V3 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;

  return result;
}
V3 &
operator+=(V3 & a, V3 b)
{
  a = a + b;
  return a;
}


V3
operator-(V3 vec)
{
  V3 result;
  result.x = -vec.x;
  result.y = -vec.y;
  result.z = -vec.z;

  return result;
}

V3
operator-(float c, V3 vec)
{
  V3 result;
  result.x = c - vec.x;
  result.y = c - vec.y;
  result.z = c - vec.z;

  return result;
}
V3
operator-(V3 vec, float c)
{
  V3 result;
  result.x = vec.x - c;
  result.y = vec.y - c;
  result.z = vec.z - c;

  return result;
}
V3 &
operator-=(V3 & vec, float c)
{
  vec = vec - c;
  return vec;
}

V3
operator-(V3 a, V3 b)
{
  V3 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;

  return result;
}
V3 &
operator-=(V3 & a, V3 b)
{
  a = a - b;
  return a;
}


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


V4
operator*(float c, V4 vec)
{
  V4 result;
  result.w = c * vec.w;
  result.x = c * vec.x;
  result.y = c * vec.y;
  result.z = c * vec.z;

  return result;
}
V4
operator*(V4 vec, float c)
{
  V4 result = c * vec;
  return result;
}
V4 &
operator*=(V4 & vec, float c)
{
  vec = vec * c;
  return vec;
}


V4
operator/(float c, V4 vec)
{
  V4 result;
  result.w = c / vec.w;
  result.x = c / vec.x;
  result.y = c / vec.y;
  result.z = c / vec.z;

  return result;
}
V4
operator/(V4 vec, float c)
{
  V4 result;
  result.w = vec.w / c;
  result.x = vec.x / c;
  result.y = vec.y / c;
  result.z = vec.z / c;

  return result;
}
V4 &
operator/=(V4 & vec, float c)
{
  vec = vec / c;
  return vec;
}


V4
operator+(float c, V4 vec)
{
  V4 result;
  result.w = c + vec.w;
  result.x = c + vec.x;
  result.y = c + vec.y;
  result.z = c + vec.z;

  return result;
}
V4
operator+(V4 vec, float c)
{
  V4 result = c + vec;
  return result;
}
V4 &
operator+=(V4 & vec, float c)
{
  vec = vec + c;
  return vec;
}

V4
operator+(V4 a, V4 b)
{
  V4 result;
  result.w = a.w + b.w;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;

  return result;
}
V4 &
operator+=(V4 & a, V4 b)
{
  a = a + b;
  return a;
}


V4
operator-(V4 vec)
{
  V4 result;
  result.w = -vec.w;
  result.x = -vec.x;
  result.y = -vec.y;
  result.z = -vec.z;

  return result;
}

V4
operator-(float c, V4 vec)
{
  V4 result;
  result.w = c - vec.w;
  result.x = c - vec.x;
  result.y = c - vec.y;
  result.z = c - vec.z;

  return result;
}
V4
operator-(V4 vec, float c)
{
  V4 result;
  result.w = vec.w - c;
  result.x = vec.x - c;
  result.y = vec.y - c;
  result.z = vec.z - c;

  return result;
}
V4 &
operator-=(V4 & vec, float c)
{
  vec = vec - c;
  return vec;
}

V4
operator-(V4 a, V4 b)
{
  V4 result;
  result.w = a.w - b.w;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;

  return result;
}
V4 &
operator-=(V4 & a, V4 b)
{
  a = a - b;
  return a;
}

union PixelColor
{
  struct
  {
    uint8_t b;
    uint8_t g;
    uint8_t r;
  };
  // Pad to 32 bits for SDL :(
  uint32_t _;
};


V3
remove_alpha(V4 vec)
{
  V3 result;
  result.r = vec.r;
  result.g = vec.g;
  result.b = vec.b;
  return result;
}


V3
pixel_color_to_V3(PixelColor color)
{
  V3 result;
  result.r = color.r;
  result.g = color.g;
  result.b = color.b;
  return result;
}


PixelColor
to_color(V3 vec)
{
  PixelColor result;
  result.r = vec.r;
  result.g = vec.g;
  result.b = vec.b;
  return result;
}