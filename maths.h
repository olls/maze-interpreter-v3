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


V3
operator*(uint32_t c, V3 vec)
{
  V3 result;
  result.x = c * vec.x;
  result.y = c * vec.y;
  result.z = c * vec.z;

  return result;
}
V3
operator*(V3 vec, uint32_t c)
{
  V3 result = c * vec;
  return result;
}
V3 &
operator*=(V3 & vec, uint32_t c)
{
  vec = vec * c;
  return vec;
}


V3
operator/(uint32_t c, V3 vec)
{
  V3 result;
  result.x = c / vec.x;
  result.y = c / vec.y;
  result.z = c / vec.z;

  return result;
}
V3
operator/(V3 vec, uint32_t c)
{
  V3 result;
  result.x = vec.x / c;
  result.y = vec.y / c;
  result.z = vec.z / c;

  return result;
}
V3 &
operator/=(V3 & vec, uint32_t c)
{
  vec = vec / c;
  return vec;
}


V3
operator+(uint32_t c, V3 vec)
{
  V3 result;
  result.x = c + vec.x;
  result.y = c + vec.y;
  result.z = c + vec.z;

  return result;
}
V3
operator+(V3 vec, uint32_t c)
{
  V3 result = c + vec;
  return result;
}
V3 &
operator+=(V3 & vec, uint32_t c)
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
operator-(uint32_t c, V3 vec)
{
  V3 result;
  result.x = c - vec.x;
  result.y = c - vec.y;
  result.z = c - vec.z;

  return result;
}
V3
operator-(V3 vec, uint32_t c)
{
  V3 result;
  result.x = vec.x - c;
  result.y = vec.y - c;
  result.z = vec.z - c;

  return result;
}
V3 &
operator-=(V3 & vec, uint32_t c)
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


V4
operator*(uint32_t c, V4 vec)
{
  V4 result;
  result.w = c * vec.w;
  result.x = c * vec.x;
  result.y = c * vec.y;
  result.z = c * vec.z;

  return result;
}
V4
operator*(V4 vec, uint32_t c)
{
  V4 result = c * vec;
  return result;
}
V4 &
operator*=(V4 & vec, uint32_t c)
{
  vec = vec * c;
  return vec;
}


V4
operator/(uint32_t c, V4 vec)
{
  V4 result;
  result.w = c / vec.w;
  result.x = c / vec.x;
  result.y = c / vec.y;
  result.z = c / vec.z;

  return result;
}
V4
operator/(V4 vec, uint32_t c)
{
  V4 result;
  result.w = vec.w / c;
  result.x = vec.x / c;
  result.y = vec.y / c;
  result.z = vec.z / c;

  return result;
}
V4 &
operator/=(V4 & vec, uint32_t c)
{
  vec = vec / c;
  return vec;
}


V4
operator+(uint32_t c, V4 vec)
{
  V4 result;
  result.w = c + vec.w;
  result.x = c + vec.x;
  result.y = c + vec.y;
  result.z = c + vec.z;

  return result;
}
V4
operator+(V4 vec, uint32_t c)
{
  V4 result = c + vec;
  return result;
}
V4 &
operator+=(V4 & vec, uint32_t c)
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
operator-(uint32_t c, V4 vec)
{
  V4 result;
  result.w = c - vec.w;
  result.x = c - vec.x;
  result.y = c - vec.y;
  result.z = c - vec.z;

  return result;
}
V4
operator-(V4 vec, uint32_t c)
{
  V4 result;
  result.w = vec.w - c;
  result.x = vec.x - c;
  result.y = vec.y - c;
  result.z = vec.z - c;

  return result;
}
V4 &
operator-=(V4 & vec, uint32_t c)
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


V4
decomposeColor(uint32_t color)
{
  V4 vec;
  vec.a = (color >> 24) & 0xFF;
  vec.r = (color >> 16) & 0xFF;
  vec.g = (color >> 8) & 0xFF;
  vec.b = (color >> 0) & 0xFF;

  return vec;
}

uint32_t
composeColor(V4 vec)
{
  uint32_t color = (((uint32_t)vec.a << 24) |
                    ((uint32_t)vec.r << 16) |
                    ((uint32_t)vec.g << 8) |
                    ((uint32_t)vec.b << 0));

  return color;
}
