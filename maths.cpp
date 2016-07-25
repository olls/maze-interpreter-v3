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
