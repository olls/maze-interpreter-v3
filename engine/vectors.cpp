vec2
Vec2(r32 x, r32 y)
{
  return (vec2){x, y};
}

vec2
Vec2(r64 x, r64 y)
{
  return (vec2){(r32)x, (r32)y};
}

vec2
Vec2(u32 x, u32 y)
{
  return (vec2){(r32)x, (r32)y};
}

vec2
Vec2(r32 x)
{
  return (vec2){x, x};
}


vec3
Vec3(r32 x, r32 y, r32 z)
{
  return (vec3){x, y, z};
}

vec3
Vec3(r64 x, r64 y, r64 z)
{
  return (vec3){(r32)x, (r32)y, (r32)z};
}

vec3
Vec3(u32 x, u32 y, u32 z)
{
  return (vec3){(r32)x, (r32)y, (r32)z};
}

vec3
Vec3(r32 x)
{
  return (vec3){x, x, x};
}


vec4
Vec4(r32 w, r32 x, r32 y, r32 z)
{
  return (vec4){w, x, y, z};
}

vec4
Vec4(r64 w, r64 x, r64 y, r64 z)
{
  return (vec4){(r32)w, (r32)x, (r32)y, (r32)z};
}

vec4
Vec4(u32 w, u32 x, u32 y, u32 z)
{
  return (vec4){(r32)w, (r32)x, (r32)y, (r32)z};
}

vec4
Vec4(r32 x)
{
  return (vec4){x, x, x, x};
}


b32
operator==(vec2 a, vec2 b)
{
  b32 result = ((a.x == b.x) &&
                (a.y == b.y));
  return result;
}
b32
operator!=(vec2 a, vec2 b)
{
  b32 result = !(a == b);
  return result;
}

b32
operator>(vec2 a, vec2 b)
{
  b32 result = ((a.x > b.x) &&
                 (a.y > b.y));
  return result;
}
b32
operator<(vec2 a, vec2 b)
{
  b32 result = ((a.x < b.x) &&
                (a.y < b.y));
  return result;
}

b32
operator>=(vec2 a, vec2 b)
{
  b32 result = ((a.x >= b.x) &&
                (a.y >= b.y));
  return result;
}
b32
operator<=(vec2 a, vec2 b)
{
  b32 result = ((a.x <= b.x) &&
                (a.y <= b.y));
  return result;
}

vec2
operator*(r32 c, vec2 vec)
{
  vec2 result;
  result.x = c * vec.x;
  result.y = c * vec.y;
  return result;
}
vec2
operator*(vec2 vec, r32 c)
{
  vec2 result = c * vec;
  return result;
}
vec2 &
operator*=(vec2 &vec, r32 c)
{
  vec = c * vec;
  return vec;
}

vec2
operator*(vec2 a, vec2 b)
{
  vec2 result;
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  return result;
}
vec2 &
operator*=(vec2 &a, vec2 b)
{
  a = a * b;
  return a;
}

vec2
operator/(r32 c, vec2 vec)
{
  vec2 result;
  result.x = c / vec.x;
  result.y = c / vec.y;
  return result;
}
vec2
operator/(vec2 vec, r32 c)
{
  vec2 result;
  result.x = vec.x / c;
  result.y = vec.y / c;
  return result;
}
vec2
operator/(vec2 a, vec2 b)
{
  vec2 result;
  result.x = a.x / b.x;
  result.y = a.y / b.y;
  return result;
}
vec2 &
operator/=(vec2 &a, r32 b)
{
  a = a / b;
  return a;
}

vec2
operator+(r32 c, vec2 vec)
{
  vec2 result;
  result.x = c + vec.x;
  result.y = c + vec.y;
  return result;
}
vec2
operator+(vec2 vec, r32 c)
{
  vec2 result = c + vec;
  return result;
}
vec2 &
operator+=(vec2 &vec, r32 c)
{
  vec = c + vec;
  return vec;
}

vec2
operator+(vec2 a, vec2 b)
{
  vec2 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return result;
}
vec2 &
operator+=(vec2 &a, vec2 b)
{
  a = a + b;
  return a;
}

vec2
operator-(vec2 vec)
{
  vec2 result;
  result.x = -vec.x;
  result.y = -vec.y;
  return result;
}

vec2
operator-(r32 c, vec2 vec)
{
  vec2 result;
  result.x = c - vec.x;
  result.y = c - vec.y;
  return result;
}
vec2
operator-(vec2 vec, r32 c)
{
  vec2 result;
  result.x = vec.x - c;
  result.y = vec.y - c;
  return result;
}
vec2 &
operator-=(vec2 &vec, r32 c)
{
  vec = vec - c;
  return vec;
}

vec2
operator-(vec2 a, vec2 b)
{
  vec2 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  return result;
}
vec2 &
operator-=(vec2 &a, vec2 b)
{
  a = a - b;
  return a;
}

vec2
min_vec2(vec2 a, vec2 b)
{
  vec2 result;
  result.x = min(a.x, b.x);
  result.y = min(a.y, b.y);
  return result;
}

vec2
max_vec2(vec2 a, vec2 b)
{
  vec2 result;
  result.x = max(a.x, b.x);
  result.y = max(a.y, b.y);
  return result;
}

vec2
round_down(vec2 vec)
{
  vec2 result;
  result.x = round_down(vec.x);
  result.y = round_down(vec.y);
  return result;
}

r32
length_sq(vec2 vec)
{
  r32 result = (vec.x * vec.x) + (vec.y * vec.y);
  return result;
}

vec2
unit_vector(vec2 vec)
{
  vec2 result = {0};
  r32 length = sqrt(length_sq(vec));
  if (length != 0)
  {
    result = vec / length;
  }
  return result;
}

vec2
vector_direction(vec2 vec)
{
  vec2 result = vec;
  result.x = div_or_0(result.x, abs(result.x));
  result.y = div_or_0(result.y, abs(result.y));
  return result;
}
vec2
vector_direction_or_1(vec2 vec)
{
  vec2 result = vec;
  result.x = div_or_1(result.x, abs(result.x));
  result.y = div_or_1(result.y, abs(result.y));
  return result;
}

vec2
vector_tangent(vec2 vec)
{
  vec2 result;
  result.x =  vec.y;
  result.y = -vec.x;
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
Rectangle
operator*(Rectangle rect, vec2 vec)
{
  Rectangle result;
  result.start = rect.start * vec;
  result.end = rect.end * vec;
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
Rectangle
operator/(Rectangle rect, vec2 vec)
{
  Rectangle result;
  result.start = rect.start / vec;
  result.end = rect.end / vec;
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
operator+(Rectangle rect, vec2 vec)
{
  Rectangle result;
  result.start = rect.start + vec;
  result.end = rect.end + vec;
  return result;
}
Rectangle
operator+(vec2 vec, Rectangle rect)
{
  Rectangle result = rect + vec;
  return result;
}
Rectangle &
operator+=(Rectangle &rect, vec2 vec)
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
operator-(vec2 c, Rectangle rect)
{
  Rectangle result;
  result.start = c - rect.start;
  result.end = c - rect.end;
  return result;
}
Rectangle
operator-(Rectangle rect, vec2 c)
{
  Rectangle result;
  result.start = rect.start - c;
  result.end = rect.end - c;
  return result;
}
Rectangle &
operator-=(Rectangle &rect, vec2 c)
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
rectangle(vec2 start, vec2 size)
{
  Rectangle result;
  result.start = start;
  result.end = result.start + size;
  return result;
}

Rectangle
radius_rectangle(vec2 center, r32 radius)
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
in_rectangle(vec2 test, Rectangle rect)
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
  result.start = max_vec2(rect.start, bound.start);
  result.end   = min_vec2(rect.end,   bound.end);
  return result;
}

Rectangle
get_overlap(Rectangle a, Rectangle b)
{
  Rectangle result = crop_to(a, b);
  return result;
}

vec2
size(Rectangle rect)
{
  vec2 result;
  result = rect.end - rect.start;
  return result;
}

vec2
get_center(Rectangle rect)
{
  vec2 result = rect.start + size(rect)*0.5;
  return result;
}

Rectangle
get_top_left(Rectangle rect)
{
  Rectangle result;
  vec2 center = get_center(rect);
  result.start = (vec2){rect.start.x, center.y};
  result.end   = (vec2){center.x, rect.end.y};
  return result;
}

Rectangle
get_top_right(Rectangle rect)
{
  Rectangle result;
  vec2 center = get_center(rect);
  result.start = center;
  result.end   = rect.end;
  return result;
}

Rectangle
get_bottom_right(Rectangle rect)
{
  Rectangle result;
  vec2 center = get_center(rect);
  result.start = (vec2){center.x, rect.start.y};
  result.end   = (vec2){rect.end.x, center.y};
  return result;
}

Rectangle
get_bottom_left(Rectangle rect)
{
  Rectangle result;
  vec2 center = get_center(rect);
  result.start = rect.start;
  result.end   = center;
  return result;
}

Rectangle
get_segment(Rectangle rect, vec2 pos, vec2 segments)
{
  vec2 seg_size = size(rect) / segments;

  Rectangle result;
  result.start = rect.start + seg_size*pos;
  result.end = result.start + seg_size;

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

Rectangle
get_horizontal_fraction(Rectangle rect, r32 frac)
{
  Rectangle result;
  result.start = rect.start;
  result.end = (vec2){
    result.start.x + (rect.end.x-rect.start.x)*frac,
    rect.end.y
  };
  return result;
}
Rectangle
get_nth_horizontal_fraction(Rectangle rect, r32 frac, u32 n)
{
  Rectangle result = get_horizontal_fraction(rect, frac);
  result += (vec2){(rect.end.x-rect.start.x)*frac*n, 0};
  return result;
}

Rectangle
get_vertical_fraction(Rectangle rect, r32 frac)
{
  Rectangle result;
  result.start = rect.start;
  result.end = (vec2){
    rect.end.x,
    result.start.y + (rect.end.y-rect.start.y)*frac
  };
  return result;
}
Rectangle
get_nth_vertical_fraction(Rectangle rect, r32 frac, u32 n)
{
  Rectangle result = get_vertical_fraction(rect, frac);
  result += (vec2){0, (rect.end.y-rect.start.y)*frac*n};
  return result;
}

Rectangle
get_union(Rectangle a, Rectangle b)
{
  Rectangle result;
  result.start = min_vec2(a.start, b.start);
  result.end   = max_vec2(a.end,   b.end);
  return result;
}


b32
operator==(vec3 a, vec3 b)
{
  b32 result = ((a.x == b.x) &&
                (a.y == b.y) &&
                (a.z == b.z));
  return result;
}
b32
operator!=(vec3 a, vec3 b)
{
  b32 result = !(a == b);
  return result;
}
vec3
operator*(r32 c, vec3 vec)
{
  vec3 result;
  result.x = c * vec.x;
  result.y = c * vec.y;
  result.z = c * vec.z;
  return result;
}
vec3
operator*(vec3 vec, r32 c)
{
  vec3 result = c * vec;
  return result;
}
vec3 &
operator*=(vec3 &vec, r32 c)
{
  vec = c * vec;
  return vec;
}

vec3
operator*(vec3 a, vec3 b)
{
  vec3 result;
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  result.z = a.z * b.z;
  return result;
}
vec3 &
operator*=(vec3 &a, vec3 b)
{
  a = a * b;
  return a;
}

vec3
operator/(r32 c, vec3 vec)
{
  vec3 result;
  result.x = c / vec.x;
  result.y = c / vec.y;
  result.z = c / vec.z;
  return result;
}
vec3
operator/(vec3 vec, r32 c)
{
  vec3 result;
  result.x = vec.x / c;
  result.y = vec.y / c;
  result.z = vec.z / c;
  return result;
}
vec3 &
operator/=(vec3 &vec, r32 c)
{
  vec = vec / c;
  return vec;
}

vec3
operator+(r32 c, vec3 vec)
{
  vec3 result;
  result.x = c + vec.x;
  result.y = c + vec.y;
  result.z = c + vec.z;
  return result;
}
vec3
operator+(vec3 vec, r32 c)
{
  vec3 result = c + vec;
  return result;
}
vec3 &
operator+=(vec3 &vec, r32 c)
{
  vec = c + vec;
  return vec;
}

vec3
operator+(vec3 a, vec3 b)
{
  vec3 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  return result;
}
vec3 &
operator+=(vec3 &a, vec3 b)
{
  a = a + b;
  return a;
}

vec3
operator-(vec3 vec)
{
  vec3 result;
  result.x = -vec.x;
  result.y = -vec.y;
  result.z = -vec.z;
  return result;
}

vec3
operator-(r32 c, vec3 vec)
{
  vec3 result;
  result.x = c - vec.x;
  result.y = c - vec.y;
  result.z = c - vec.z;
  return result;
}
vec3
operator-(vec3 vec, r32 c)
{
  vec3 result;
  result.x = vec.x - c;
  result.y = vec.y - c;
  result.z = vec.z - c;
  return result;
}
vec3 &
operator-=(vec3 &vec, r32 c)
{
  vec = vec - c;
  return vec;
}

vec3
operator-(vec3 a, vec3 b)
{
  vec3 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  return result;
}
vec3 &
operator-=(vec3 &a, vec3 b)
{
  a = a - b;
  return a;
}

vec3
min_vec3(vec3 a, vec3 b)
{
  vec3 result;
  result.x = min(a.x, b.x);
  result.y = min(a.y, b.y);
  result.z = min(a.z, b.z);
  return result;
}

vec3
max_vec3(vec3 a, vec3 b)
{
  vec3 result;
  result.x = max(a.x, b.x);
  result.y = max(a.y, b.y);
  result.z = max(a.z, b.z);
  return result;
}

vec3
round_down(vec3 vec)
{
  vec3 result;
  result.x = (s32)vec.x;
  result.y = (s32)vec.y;
  result.z = (s32)vec.z;
  return result;
}

b32
operator==(vec4 a, vec4 b)
{
  b32 result = ((a.w == b.w) &&
                (a.x == b.x) &&
                (a.y == b.y) &&
                (a.z == b.z));
  return result;
}
b32
operator!=(vec4 a, vec4 b)
{
  b32 result = !(a == b);
  return result;
}
vec4
operator*(r32 c, vec4 vec)
{
  vec4 result;
  result.w = c * vec.w;
  result.x = c * vec.x;
  result.y = c * vec.y;
  result.z = c * vec.z;
  return result;
}
vec4
operator*(vec4 vec, r32 c)
{
  vec4 result = c * vec;
  return result;
}
vec4 &
operator*=(vec4 &vec, r32 c)
{
  vec = c * vec;
  return vec;
}

vec4
operator*(vec4 a, vec4 b)
{
  vec4 result;
  result.w = a.w * b.w;
  result.x = a.x * b.x;
  result.y = a.y * b.y;
  result.z = a.z * b.z;
  return result;
}
vec4 &
operator*=(vec4 &a, vec4 b)
{
  a = a * b;
  return a;
}

vec4
operator/(r32 c, vec4 vec)
{
  vec4 result;
  result.w = c / vec.w;
  result.x = c / vec.x;
  result.y = c / vec.y;
  result.z = c / vec.z;
  return result;
}
vec4
operator/(vec4 vec, r32 c)
{
  vec4 result;
  result.w = vec.w / c;
  result.x = vec.x / c;
  result.y = vec.y / c;
  result.z = vec.z / c;
  return result;
}
vec4 &
operator/=(vec4 &vec, r32 c)
{
  vec = vec / c;
  return vec;
}

vec4
operator+(r32 c, vec4 vec)
{
  vec4 result;
  result.w = c + vec.w;
  result.x = c + vec.x;
  result.y = c + vec.y;
  result.z = c + vec.z;
  return result;
}
vec4
operator+(vec4 vec, r32 c)
{
  vec4 result = c + vec;
  return result;
}
vec4 &
operator+=(vec4 &vec, r32 c)
{
  vec = c + vec;
  return vec;
}

vec4
operator+(vec4 a, vec4 b)
{
  vec4 result;
  result.w = a.w + b.w;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  return result;
}
vec4 &
operator+=(vec4 &a, vec4 b)
{
  a = a + b;
  return a;
}

vec4
operator-(vec4 vec)
{
  vec4 result;
  result.w = -vec.w;
  result.x = -vec.x;
  result.y = -vec.y;
  result.z = -vec.z;
  return result;
}

vec4
operator-(r32 c, vec4 vec)
{
  vec4 result;
  result.w = c - vec.w;
  result.x = c - vec.x;
  result.y = c - vec.y;
  result.z = c - vec.z;
  return result;
}
vec4
operator-(vec4 vec, r32 c)
{
  vec4 result;
  result.w = vec.w - c;
  result.x = vec.x - c;
  result.y = vec.y - c;
  result.z = vec.z - c;
  return result;
}
vec4 &
operator-=(vec4 &vec, r32 c)
{
  vec = vec - c;
  return vec;
}

vec4
operator-(vec4 a, vec4 b)
{
  vec4 result;
  result.w = a.w - b.w;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  return result;
}
vec4 &
operator-=(vec4 &a, vec4 b)
{
  a = a - b;
  return a;
}

vec4
min_vec4(vec4 a, vec4 b)
{
  vec4 result;
  result.w = min(a.w, b.w);
  result.x = min(a.x, b.x);
  result.y = min(a.y, b.y);
  result.z = min(a.z, b.z);
  return result;
}

vec4
max_vec4(vec4 a, vec4 b)
{
  vec4 result;
  result.w = max(a.w, b.w);
  result.x = max(a.x, b.x);
  result.y = max(a.y, b.y);
  result.z = max(a.z, b.z);
  return result;
}

vec4
clamp(vec4 vec, r32 max)
{
  vec4 result = min_vec4(vec, (vec4){max, max, max, max});
  return result;
}

vec4
round_down(vec4 vec)
{
  vec4 result;
  result.w = (s32)vec.w;
  result.x = (s32)vec.x;
  result.y = (s32)vec.y;
  result.z = (s32)vec.z;
  return result;
}


vec4
lerp(vec4 min, r32 t, vec4 max)
{
  vec4 result;
  result.w = lerp(min.w, t, max.w);
  result.x = lerp(min.x, t, max.x);
  result.y = lerp(min.y, t, max.y);
  result.z = lerp(min.z, t, max.z);
  return result;
}


vec3
remove_alpha(vec4 vec)
{
  vec3 result;
  result.r = vec.r;
  result.g = vec.g;
  result.b = vec.b;
  return result;
}


vec3
pixel_color_to_vec3(PixelColor color)
{
  vec3 result;
  result.r = color.r;
  result.g = color.g;
  result.b = color.b;
  return result;
}


PixelColor
to_color(vec3 vec)
{
  PixelColor result;
  result.r = vec.r;
  result.g = vec.g;
  result.b = vec.b;
  return result;
}


vec4
add_color(vec4 colour, r32 x)
{
  vec4 result = colour + x;
  result.a = colour.a;
  return result;
}


vec4
get_color(s32 i = -1)
{
  static vec4 colours[] = {(vec4){1, 0.0, 0.0, 0.0},
                          (vec4){1, 0.0, 0.0, 0.5},
                          (vec4){1, 0.0, 0.0, 1.0},
                          (vec4){1, 0.0, 0.5, 0.0},
                          (vec4){1, 0.0, 0.5, 0.5},
                          (vec4){1, 0.0, 0.5, 1.0},
                          (vec4){1, 0.0, 1.0, 0.0},
                          (vec4){1, 0.0, 1.0, 0.5},
                          (vec4){1, 0.0, 1.0, 1.0},
                          (vec4){1, 0.5, 0.0, 0.0},
                          (vec4){1, 0.5, 0.0, 0.5},
                          (vec4){1, 0.5, 0.0, 1.0},
                          (vec4){1, 0.5, 0.5, 0.0},
                          (vec4){1, 0.5, 0.5, 0.5},
                          (vec4){1, 0.5, 0.5, 1.0},
                          (vec4){1, 0.5, 1.0, 0.0},
                          (vec4){1, 0.5, 1.0, 0.5},
                          (vec4){1, 0.5, 1.0, 1.0},
                          (vec4){1, 1.0, 0.0, 0.0},
                          (vec4){1, 1.0, 0.0, 0.5},
                          (vec4){1, 1.0, 0.0, 1.0},
                          (vec4){1, 1.0, 0.5, 0.0},
                          (vec4){1, 1.0, 0.5, 0.5},
                          (vec4){1, 1.0, 0.5, 1.0},
                          (vec4){1, 1.0, 1.0, 0.0},
                          (vec4){1, 1.0, 1.0, 0.5},
                          (vec4){1, 1.0, 1.0, 1.0}};

  if (i < 0)
  {
    i = rand();
  }

  i %= 27;

  return colours[i];
}


char
vector_to_compass_dir(vec2 direction)
{
  char result = 0;

  if (direction.x == 0 && direction.y == -1)
  {
    result = 'U';
  }
  else if (direction.x == 0 && direction.y == 1)
  {
    result = 'D';
  }
  else if (direction.x == -1 && direction.y == 0)
  {
    result = 'L';
  }
  else if (direction.x == 1 && direction.y == 0)
  {
    result = 'R';
  }
  else if (direction.x == 0 && direction.y == 0)
  {
    result = 'N';
  }

  return result;
}


r32
angle_from_vector(vec2 direction)
{
  r32 result = 0;

  // TODO: Once our blit_bitmap handles more than 90deg segments, use this instead...
  // result = atan2(direction.x, direction.y) * 180.0 / M_PI;

  if (direction == UP)
  {
      result = 0;
  }
  else if (direction == DOWN)
  {
      result = 180;
  }
  else if (direction == RIGHT)
  {
      result = 90;
  }
  else if (direction == LEFT)
  {
      result = 270;
  }

  return result;
}