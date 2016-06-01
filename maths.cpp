r32
squared(r32 base)
{
  r32 result = base * base;
  return result;
}


u32
clamp(u32 value, u32 min, u32 max)
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


u32
lerp(u32 min, u32 max, r32 t)
{
  u32 result = ((1 - t) * min) + (t * max);
  return result;
}



void
printF(r32 f)
{
  printf("%f\n", f);
}

void
printU(u32 u)
{
  printf("%d\n", u);
}

void
printI(s32 i)
{
  printf("%d\n", i);
}
