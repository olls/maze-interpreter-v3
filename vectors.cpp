V2
v2(r32 x)
{
  return (V2){x, x};
}

b32
operator==(V2 a, V2 b)
{
  b32 result = ((a.x == b.x) &&
                (a.y == b.y));
  return result;
}
b32
operator!=(V2 a, V2 b)
{
  b32 result = !(a == b);
  return result;
}

b32
operator>(V2 a, V2 b)
{
  b32 result = ((a.x > b.x) &&
                 (a.y > b.y));
  return result;
}
b32
operator<(V2 a, V2 b)
{
  b32 result = ((a.x < b.x) &&
                (a.y < b.y));
  return result;
}

b32
operator>=(V2 a, V2 b)
{
  b32 result = ((a.x >= b.x) &&
                (a.y >= b.y));
  return result;
}
b32
operator<=(V2 a, V2 b)
{
  b32 result = ((a.x <= b.x) &&
                (a.y <= b.y));
  return result;
}

V2
operator*(r32 c, V2 vec)
{
  V2 result;
  result.x = c * vec.x;
  result.y = c * vec.y;
  return result;
}
V2
operator*(V2 vec, r32 c)
{
  V2 result = c * vec;
  return result;
}
V2 &
operator*=(V2 &vec, r32 c)
{
  vec = c * vec;
  return vec;
}

V2
operator*(V2 a, V2 b)
{
  V2 result;
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  return result;
}
V2 &
operator*=(V2 &a, V2 b)
{
  a = a * b;
  return a;
}

V2
operator/(r32 c, V2 vec)
{
  V2 result;
  result.x = c / vec.x;
  result.y = c / vec.y;
  return result;
}
V2
operator/(V2 vec, r32 c)
{
  V2 result;
  result.x = vec.x / c;
  result.y = vec.y / c;
  return result;
}
V2
operator/(V2 a, V2 b)
{
  V2 result;
  result.x = a.x / b.x;
  result.y = a.y / b.y;
  return result;
}
V2 &
operator/=(V2 &a, r32 b)
{
  a = a / b;
  return a;
}

V2
operator+(r32 c, V2 vec)
{
  V2 result;
  result.x = c + vec.x;
  result.y = c + vec.y;
  return result;
}
V2
operator+(V2 vec, r32 c)
{
  V2 result = c + vec;
  return result;
}
V2 &
operator+=(V2 &vec, r32 c)
{
  vec = c + vec;
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
operator+=(V2 &a, V2 b)
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
operator-(r32 c, V2 vec)
{
  V2 result;
  result.x = c - vec.x;
  result.y = c - vec.y;
  return result;
}
V2
operator-(V2 vec, r32 c)
{
  V2 result;
  result.x = vec.x - c;
  result.y = vec.y - c;
  return result;
}
V2 &
operator-=(V2 &vec, r32 c)
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
operator-=(V2 &a, V2 b)
{
  a = a - b;
  return a;
}

V2
min_V2(V2 a, V2 b)
{
  V2 result;
  result.x = min(a.x, b.x);
  result.y = min(a.y, b.y);
  return result;
}

V2
max_V2(V2 a, V2 b)
{
  V2 result;
  result.x = max(a.x, b.x);
  result.y = max(a.y, b.y);
  return result;
}

V2
round_down(V2 vec)
{
  V2 result;
  result.x = (s32)vec.x;
  result.y = (s32)vec.y;
  return result;
}

r32
length_sq(V2 vec)
{
  r32 result = (vec.x * vec.x) + (vec.y * vec.y);
  return result;
}

V2
unit_vector(V2 vec)
{
  V2 result = {0};
  r32 length = sqrt(length_sq(vec));
  if (length != 0)
  {
    result = vec / length;
  }
  return result;
}

V2
vector_direction(V2 vec)
{
  V2 result = vec;
  result.x = div_or_0(result.x, abs(result.x));
  result.y = div_or_0(result.y, abs(result.y));
  return result;
}


b32
operator==(Rectangle a, Rectangle b)
{
  b32 result = ((a.start == b.start) &&
                (a.end == b.end));
  return result;
}
b32
operator!=(Rectangle a, Rectangle b)
{
  b32 result = !(a == b);
  return result;
}
Rectangle
operator*(r32 c, Rectangle rect)
{
  Rectangle result;
  result.start = c * rect.start;
  result.end = c * rect.end;
  return result;
}
Rectangle
operator*(Rectangle rect, r32 c)
{
  Rectangle result = c * rect;
  return result;
}
Rectangle &
operator*=(Rectangle &rect, r32 c)
{
  rect = c * rect;
  return rect;
}

Rectangle
operator/(r32 c, Rectangle rect)
{
  Rectangle result;
  result.start = c / rect.start;
  result.end = c / rect.end;
  return result;
}
Rectangle
operator/(Rectangle rect, r32 c)
{
  Rectangle result;
  result.start = rect.start / c;
  result.end = rect.end / c;
  return result;
}
Rectangle &
operator/=(Rectangle &rect, r32 c)
{
  rect = rect / c;
  return rect;
}

Rectangle
operator+(r32 c, Rectangle rect)
{
  Rectangle result;
  result.start = c + rect.start;
  result.end = c + rect.end;
  return result;
}
Rectangle
operator+(Rectangle rect, r32 c)
{
  Rectangle result = c + rect;
  return result;
}
Rectangle &
operator+=(Rectangle &rect, r32 c)
{
  rect = rect + c;
  return rect;
}

Rectangle
operator+(Rectangle rect, V2 vec)
{
  Rectangle result;
  result.start = rect.start + vec;
  result.end = rect.end + vec;
  return result;
}
Rectangle
operator+(V2 vec, Rectangle rect)
{
  Rectangle result = rect + vec;
  return result;
}
Rectangle &
operator+=(Rectangle &rect, V2 vec)
{
  rect = rect + vec;
  return rect;
}

Rectangle
operator+(Rectangle a, Rectangle b)
{
  Rectangle result;
  result.start = a.start + b.start;
  result.end = a.end + b.end;
  return result;
}
Rectangle &
operator+=(Rectangle &a, Rectangle b)
{
  a = a + b;
  return a;
}

Rectangle
operator-(Rectangle rect)
{
  Rectangle result;
  result.start = -rect.start;
  result.end = -rect.end;
  return result;
}

Rectangle
operator-(r32 c, Rectangle rect)
{
  Rectangle result;
  result.start = c - rect.start;
  result.end = c - rect.end;
  return result;
}
Rectangle
operator-(Rectangle rect, r32 c)
{
  Rectangle result;
  result.start = rect.start - c;
  result.end = rect.end - c;
  return result;
}
Rectangle &
operator-=(Rectangle &rect, r32 c)
{
  rect = rect - c;
  return rect;
}

Rectangle
operator-(V2 c, Rectangle rect)
{
  Rectangle result;
  result.start = c - rect.start;
  result.end = c - rect.end;
  return result;
}
Rectangle
operator-(Rectangle rect, V2 c)
{
  Rectangle result;
  result.start = rect.start - c;
  result.end = rect.end - c;
  return result;
}
Rectangle &
operator-=(Rectangle &rect, V2 c)
{
  rect = rect - c;
  return rect;
}

Rectangle
operator-(Rectangle a, Rectangle b)
{
  Rectangle result;
  result.start = a.start - b.start;
  result.end = a.end - b.end;
  return result;
}
Rectangle &
operator-=(Rectangle &a, Rectangle b)
{
  a = a - b;
  return a;
}

Rectangle
rectangle(V2 start, V2 size)
{
  Rectangle result;
  result.start = start;
  result.end = result.start + size;
  return result;
}

Rectangle
radius_rectangle(V2 center, r32 radius)
{
  Rectangle result;
  result.start = center - radius;
  result.end = center + radius;
  return result;
}

Rectangle
round_down(Rectangle rect)
{
  Rectangle result;
  result.start = round_down(rect.start);
  result.end = round_down(rect.end);
  return result;
}

b32
in_rectangle(V2 test, Rectangle rect)
{
  b32 result = ((test.x >= rect.start.x) &&
                (test.x < rect.end.x) &&
                (test.y >= rect.start.y) &&
                (test.y < rect.end.y));
  return result;
}

b32
overlaps(Rectangle a, Rectangle b)
{
  b32 result = !((b.end.x < a.start.x) ||
                 (b.start.x > a.end.x) ||
                 (b.end.y < a.start.y) ||
                 (b.start.y > a.end.y));
  return result;
}

Rectangle
crop_to(Rectangle rect, Rectangle bound)
{
  Rectangle result;
  result.start = max_V2(rect.start, bound.start);
  result.end   = min_V2(rect.end,   bound.end);
  return result;
}

Rectangle
get_overlap(Rectangle a, Rectangle b)
{
  Rectangle result = crop_to(a, b);
  return result;
}

V2
get_center(Rectangle rect)
{
  V2 result = rect.start + ((rect.end - rect.start) / 2);
  return result;
}

Rectangle
get_top_left(Rectangle rect)
{
  Rectangle result;
  V2 center = get_center(rect);
  result.start = (V2){rect.start.x, center.y};
  result.end   = (V2){center.x, rect.end.y};
  return result;
}

Rectangle
get_top_right(Rectangle rect)
{
  Rectangle result;
  V2 center = get_center(rect);
  result.start = center;
  result.end   = rect.end;
  return result;
}

Rectangle
get_bottom_right(Rectangle rect)
{
  Rectangle result;
  V2 center = get_center(rect);
  result.start = (V2){center.x, rect.start.y};
  result.end   = (V2){rect.end.x, center.y};
  return result;
}

Rectangle
get_bottom_left(Rectangle rect)
{
  Rectangle result;
  V2 center = get_center(rect);
  result.start = rect.start;
  result.end   = center;
  return result;
}

Rectangle
grow(Rectangle rect, r32 by)
{
  Rectangle result = rect;
  result.start -= by;
  result.end += by;
  return result;
}


b32
operator==(V3 a, V3 b)
{
  b32 result = ((a.x == b.x) &&
                (a.y == b.y) &&
                (a.z == b.z));
  return result;
}
b32
operator!=(V3 a, V3 b)
{
  b32 result = !(a == b);
  return result;
}
V3
operator*(r32 c, V3 vec)
{
  V3 result;
  result.x = c * vec.x;
  result.y = c * vec.y;
  result.z = c * vec.z;
  return result;
}
V3
operator*(V3 vec, r32 c)
{
  V3 result = c * vec;
  return result;
}
V3 &
operator*=(V3 &vec, r32 c)
{
  vec = c * vec;
  return vec;
}

V3
operator*(V3 a, V3 b)
{
  V3 result;
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  result.z = a.z * b.z;
  return result;
}
V3 &
operator*=(V3 &a, V3 b)
{
  a = a * b;
  return a;
}

V3
operator/(r32 c, V3 vec)
{
  V3 result;
  result.x = c / vec.x;
  result.y = c / vec.y;
  result.z = c / vec.z;
  return result;
}
V3
operator/(V3 vec, r32 c)
{
  V3 result;
  result.x = vec.x / c;
  result.y = vec.y / c;
  result.z = vec.z / c;
  return result;
}
V3 &
operator/=(V3 &vec, r32 c)
{
  vec = vec / c;
  return vec;
}

V3
operator+(r32 c, V3 vec)
{
  V3 result;
  result.x = c + vec.x;
  result.y = c + vec.y;
  result.z = c + vec.z;
  return result;
}
V3
operator+(V3 vec, r32 c)
{
  V3 result = c + vec;
  return result;
}
V3 &
operator+=(V3 &vec, r32 c)
{
  vec = c + vec;
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
operator+=(V3 &a, V3 b)
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
operator-(r32 c, V3 vec)
{
  V3 result;
  result.x = c - vec.x;
  result.y = c - vec.y;
  result.z = c - vec.z;
  return result;
}
V3
operator-(V3 vec, r32 c)
{
  V3 result;
  result.x = vec.x - c;
  result.y = vec.y - c;
  result.z = vec.z - c;
  return result;
}
V3 &
operator-=(V3 &vec, r32 c)
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
operator-=(V3 &a, V3 b)
{
  a = a - b;
  return a;
}

V3
min_V3(V3 a, V3 b)
{
  V3 result;
  result.x = min(a.x, b.x);
  result.y = min(a.y, b.y);
  result.z = min(a.z, b.z);
  return result;
}

V3
max_V3(V3 a, V3 b)
{
  V3 result;
  result.x = max(a.x, b.x);
  result.y = max(a.y, b.y);
  result.z = max(a.z, b.z);
  return result;
}

V3
round_down(V3 vec)
{
  V3 result;
  result.x = (s32)vec.x;
  result.y = (s32)vec.y;
  result.z = (s32)vec.z;
  return result;
}

b32
operator==(V4 a, V4 b)
{
  b32 result = ((a.w == b.w) &&
                (a.x == b.x) &&
                (a.y == b.y) &&
                (a.z == b.z));
  return result;
}
b32
operator!=(V4 a, V4 b)
{
  b32 result = !(a == b);
  return result;
}
V4
operator*(r32 c, V4 vec)
{
  V4 result;
  result.w = c * vec.w;
  result.x = c * vec.x;
  result.y = c * vec.y;
  result.z = c * vec.z;
  return result;
}
V4
operator*(V4 vec, r32 c)
{
  V4 result = c * vec;
  return result;
}
V4 &
operator*=(V4 &vec, r32 c)
{
  vec = c * vec;
  return vec;
}

V4
operator*(V4 a, V4 b)
{
  V4 result;
  result.w = a.w * b.w;
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  result.z = a.z * b.z;
  return result;
}
V4 &
operator*=(V4 &a, V4 b)
{
  a = a * b;
  return a;
}

V4
operator/(r32 c, V4 vec)
{
  V4 result;
  result.w = c / vec.w;
  result.x = c / vec.x;
  result.y = c / vec.y;
  result.z = c / vec.z;
  return result;
}
V4
operator/(V4 vec, r32 c)
{
  V4 result;
  result.w = vec.w / c;
  result.x = vec.x / c;
  result.y = vec.y / c;
  result.z = vec.z / c;
  return result;
}
V4 &
operator/=(V4 &vec, r32 c)
{
  vec = vec / c;
  return vec;
}

V4
operator+(r32 c, V4 vec)
{
  V4 result;
  result.w = c + vec.w;
  result.x = c + vec.x;
  result.y = c + vec.y;
  result.z = c + vec.z;
  return result;
}
V4
operator+(V4 vec, r32 c)
{
  V4 result = c + vec;
  return result;
}
V4 &
operator+=(V4 &vec, r32 c)
{
  vec = c + vec;
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
operator+=(V4 &a, V4 b)
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
operator-(r32 c, V4 vec)
{
  V4 result;
  result.w = c - vec.w;
  result.x = c - vec.x;
  result.y = c - vec.y;
  result.z = c - vec.z;
  return result;
}
V4
operator-(V4 vec, r32 c)
{
  V4 result;
  result.w = vec.w - c;
  result.x = vec.x - c;
  result.y = vec.y - c;
  result.z = vec.z - c;
  return result;
}
V4 &
operator-=(V4 &vec, r32 c)
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
operator-=(V4 &a, V4 b)
{
  a = a - b;
  return a;
}

V4
min_V4(V4 a, V4 b)
{
  V4 result;
  result.w = min(a.w, b.w);
  result.x = min(a.x, b.x);
  result.y = min(a.y, b.y);
  result.z = min(a.z, b.z);
  return result;
}

V4
max_V4(V4 a, V4 b)
{
  V4 result;
  result.w = max(a.w, b.w);
  result.x = max(a.x, b.x);
  result.y = max(a.y, b.y);
  result.z = max(a.z, b.z);
  return result;
}

V4
clamp(V4 vec, u32 max)
{
  V4 result = min_V4(vec, (V4){max, max, max, max});
  return result;
}

V4
round_down(V4 vec)
{
  V4 result;
  result.w = (s32)vec.w;
  result.x = (s32)vec.x;
  result.y = (s32)vec.y;
  result.z = (s32)vec.z;
  return result;
}


V4
lerp(V4 min, r32 t, V4 max)
{
  V4 result;
  result.w = lerp(min.w, t, max.w);
  result.x = lerp(min.x, t, max.x);
  result.y = lerp(min.y, t, max.y);
  result.z = lerp(min.z, t, max.z);
  return result;
}


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


V4
add_color(V4 colour, r32 x)
{
  V4 result = colour + x;
  result.a = colour.a;
  return result;
}


V4
get_color(s32 i = -1)
{
  static V4 colors[] = {(V4){1, 0.0, 0.0, 0.0},
                        (V4){1, 0.0, 0.0, 0.5},
                        (V4){1, 0.0, 0.0, 1.0},
                        (V4){1, 0.0, 0.5, 0.0},
                        (V4){1, 0.0, 0.5, 0.5},
                        (V4){1, 0.0, 0.5, 1.0},
                        (V4){1, 0.0, 1.0, 0.0},
                        (V4){1, 0.0, 1.0, 0.5},
                        (V4){1, 0.0, 1.0, 1.0},
                        (V4){1, 0.5, 0.0, 0.0},
                        (V4){1, 0.5, 0.0, 0.5},
                        (V4){1, 0.5, 0.0, 1.0},
                        (V4){1, 0.5, 0.5, 0.0},
                        (V4){1, 0.5, 0.5, 0.5},
                        (V4){1, 0.5, 0.5, 1.0},
                        (V4){1, 0.5, 1.0, 0.0},
                        (V4){1, 0.5, 1.0, 0.5},
                        (V4){1, 0.5, 1.0, 1.0},
                        (V4){1, 1.0, 0.0, 0.0},
                        (V4){1, 1.0, 0.0, 0.5},
                        (V4){1, 1.0, 0.0, 1.0},
                        (V4){1, 1.0, 0.5, 0.0},
                        (V4){1, 1.0, 0.5, 0.5},
                        (V4){1, 1.0, 0.5, 1.0},
                        (V4){1, 1.0, 1.0, 0.0},
                        (V4){1, 1.0, 1.0, 0.5},
                        (V4){1, 1.0, 1.0, 1.0}};

  if (i < 0)
  {
    i = rand();
  }

  i %= 27;

  return colors[i];
}
