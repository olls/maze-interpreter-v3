float
squared(float base)
{
  float result = base * base;
  return result;
}


uint32_t
clamp(uint32_t value, uint32_t min, uint32_t max)
{
  uint32_t result = value;
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


uint32_t
lerp(uint32_t min, uint32_t max, float t)
{
  uint32_t result = ((1 - t) * min) + (t * max);
  return result;
}