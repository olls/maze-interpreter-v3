s32
min(s32 a, s32 b)
{
  s32 result = a;
  if (b < a)
  {
    result = b;
  }
  return result;
}
u32
min(u32 a, u32 b)
{
  u32 result = a;
  if (b < a)
  {
    result = b;
  }
  return result;
}
r32
min(r32 a, r32 b)
{
  r32 result = a;
  if (b < a)
  {
    result = b;
  }
  return result;
}

s32
max(s32 a, s32 b)
{
  s32 result = a;
  if (b > a)
  {
    result = b;
  }
  return result;
}
u32
max(u32 a, u32 b)
{
  s32 result = a;
  if (b > a)
  {
    result = b;
  }
  return result;
}
r32
max(r32 a, r32 b)
{
  r32 result = a;
  if (b > a)
  {
    result = b;
  }
  return result;
}


s32
abs(s32 x)
{
  s32 result = x;
  if (x < 0)
  {
    result = -x;
  }
  return result;
}
r32
abs(r32 x)
{
  r32 result = x;
  if (x < 0)
  {
    result = -x;
  }
  return result;
}


s32
round_down(r32 x)
{
  s32 result;
  if (x < 0)
  {
    x -= 1.0;
  }
  result = (s32)x;

  return result;
}


r32
squared(r32 base)
{
  r32 result = base * base;
  return result;
}


u32
clamp(u32 min, u32 value, u32 max)
{
  u32 result = value;
  if (value > max)
  {
    result = max;
  }
  else if (value < min)
  {
    result = min;
  }
  return result;
}


r32
clamp(r32 min, r32 value, r32 max)
{
  r32 result = value;
  if (value > max)
  {
    result = max;
  }
  else if (value < min)
  {
    result = min;
  }
  return result;
}


u32
lerp(u32 min, r32 t, u32 max)
{
  u32 result = ((1 - t) * min) + (t * max);
  return result;
}


r32
lerp(r32 min, r32 t, r32 max)
{
  r32 result = ((1 - t) * min) + (t * max);
  return result;
}


r32
div_or_0(r32 n, r32 d)
{
  r32 result = 0;
  if (d != 0)
  {
    result = n / d;
  }
  return result;
}
r32
div_or_1(r32 n, r32 d)
{
  r32 result = 1;
  if (d != 0)
  {
    result = n / d;
  }
  return result;
}

s32
sign(s32 a)
{
  s32 result = a >= 0 ? 1 : -1;
  return result;
}
r32
sign(r32 a)
{
  r32 result = a >= 0 ? 1 : -1;
  return result;
}


void
printF(LoggingChannel channel, r32 f)
{
  log(channel, "%f", f);
}

void
printU(LoggingChannel channel, u32 u)
{
  log(channel, "%d", u);
}

void
printI(LoggingChannel channel, s32 i)
{
  log(channel, "%d", i);
}
