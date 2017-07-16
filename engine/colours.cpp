vec4
rgb_to_hsv(vec4 rgb)
{
  vec4 hsv;
  hsv.a = rgb.a;

  r32 min_c = min(rgb.r, min(rgb.g, rgb.b));
  r32 max_c = max(rgb.r, max(rgb.g, rgb.b));
  hsv.v = max_c;

  r32 delta = max_c - min_c;

  if (max_c != 0)
  {
    hsv.s = delta / max_c;

    if (delta == 0)
      hsv.h = 0;
    else if (rgb.r == max_c)
      hsv.h = (rgb.g - rgb.b) / delta;      // between yellow & magenta
    else if (rgb.g == max_c)
      hsv.h = 2 + (rgb.b - rgb.r) / delta;  // between cyan & yellow
    else
      hsv.h = 4 + (rgb.r - rgb.g) / delta;  // between magenta & cyan

    hsv.h *= 60.0f;

    if (hsv.h < 0)
      hsv.h += 360;
  }
  else
  {
    hsv.s = 0;
    hsv.h = -1;
  }

  return hsv;
}


vec4
hsv_to_rgb(vec4 hsv)
{
  vec4 rgb;
  rgb.a = hsv.a;

  u32 i;
  r32 f, p, q, t;

  if (hsv.s == 0)
  {
    rgb.r = rgb.g = rgb.b = hsv.v;  // Grey
    return rgb;
  }

  r32 h = hsv.h / 60.0f;
  i = floor(h);
  f = h - i;
  p = hsv.v * (1.0f - hsv.s);
  q = hsv.v * (1.0f - hsv.s * f);
  t = hsv.v * (1.0f - hsv.s * (1.0f - f));

  switch (i)
  {
    case 0:
      rgb.r = hsv.v;
      rgb.g = t;
      rgb.b = p;
      break;
    case 1:
      rgb.r = q;
      rgb.g = hsv.v;
      rgb.b = p;
      break;
    case 2:
      rgb.r = p;
      rgb.g = hsv.v;
      rgb.b = t;
      break;
    case 3:
      rgb.r = p;
      rgb.g = q;
      rgb.b = hsv.v;
      break;
    case 4:
      rgb.r = t;
      rgb.g = p;
      rgb.b = hsv.v;
      break;
    default:
      rgb.r = hsv.v;
      rgb.g = p;
      rgb.b = q;
      break;
  }

  return rgb;
}


vec4
shift_hue(vec4 color, u32 degrees)
{
  vec4 hsv = rgb_to_hsv(color);
  hsv.h = ((u32)hsv.h + degrees) % 360 + (hsv.h - (u32)hsv.h);
  vec4 result = hsv_to_rgb(hsv);

  return result;
}