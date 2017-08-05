void
register_game_logging_channels(const u8 **GAME_LOGGING_CHANNEL_NAMES)
{
  GAME_LOGGING_CHANNELS = GAME_LOGGING_CHANNEL_NAMES;
}


b32
channel_enabled(ENGINELoggingChannel channel)
{
  switch(channel)
  {
    case L_Main:
    case L_Bitmap:
    case L_SVG:
    case L_XML:
    case L_XMLTokens:
    case L_Font:
    case L_OpenGL:
    case L_OpenGL_Debug:
    case L_Layouter:
    // case L_OpenType:
    {
      return false;
    } break;

    default:
    {
      return true;
    } break;
  }
}


POLYMORPHIC_LOGGING_ENDPOINT(
void
log_s, {
  printf("%s", buf);
},
const u8 *text, ...)


POLYMORPHIC_LOGGING_ENDPOINT(
void
log, {
  printf("\e[01;3%dm%s\e[0m -> %s\n", channel % 8, channel_name, buf);
},
const u8 *text, ...)


POLYMORPHIC_LOGGING_ENDPOINT(
void
log_ind,
{
  printf("\e[01;3%dm%s\e[0m -> %*s%s\n", channel % 8, channel_name, n, " ", buf);
},
u32 n, const u8 *text, ...)


POLYMORPHIC_LOGGING_FUNCTION(
void
log_d,
{
  const u32 s_length = 16;
  u8 s[s_length] = {};

  switch (vector_to_compass_dir(direction))
  {
    case ('U'): copy_string(s, u8("UP"), s_length);
      break;
    case ('D'): copy_string(s, u8("DOWN"), s_length);
      break;
    case ('L'): copy_string(s, u8("LEFT"), s_length);
      break;
    case ('R'): copy_string(s, u8("RIGHT"), s_length);
      break;
    case ('N'): copy_string(s, u8("STATIONARY"), s_length);
      break;
  }

  if (s[0])
  {
    log_s(channel, s);
  }
},
vec2 direction)


POLYMORPHIC_LOGGING_FUNCTION(
void
log,
{
  log(channel, u8("(%f, %f)"), vec.x, vec.y);
},
vec2 vec)

POLYMORPHIC_LOGGING_FUNCTION(
void
log,
{
  log(channel, u8("(%f, %f, %f)"), vec.x, vec.y, vec.z);
},
vec3 vec)

POLYMORPHIC_LOGGING_FUNCTION(
void
log,
{
  log(channel, u8("(%f, %f, %f, %f)"), vec.w, vec.x, vec.y, vec.z);
},
vec4 vec)

POLYMORPHIC_LOGGING_FUNCTION(
void
log,
{
  log(channel, u8("((%f, %f), (%f, %f))"), rect.start.x, rect.start.y, rect.end.x, rect.end.y);
},
Rectangle rect)