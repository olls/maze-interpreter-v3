void
register_game_logging_channels(const char **GAME_LOGGING_CHANNEL_NAMES)
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
const char *text, ...)


POLYMORPHIC_LOGGING_ENDPOINT(
void
log, {
  printf("\e[01;3%dm%s\e[0m -> %s\n", channel % 8, channel_name, buf);
},
const char *text, ...)


POLYMORPHIC_LOGGING_ENDPOINT(
void
log_ind,
{
  printf("\e[01;3%dm%s\e[0m -> %*s%s\n", channel % 8, channel_name, n, " ", buf);
},
u32 n, const char *text, ...)


POLYMORPHIC_LOGGING_FUNCTION(
void
log_d,
{
  char s[16] = {};

  switch (vector_to_compass_dir(direction))
  {
    case ('U'): strcpy(s, "UP");
      break;
    case ('D'): strcpy(s, "DOWN");
      break;
    case ('L'): strcpy(s, "LEFT");
      break;
    case ('R'): strcpy(s, "RIGHT");
      break;
    case ('N'): strcpy(s, "STATIONARY");
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
  log(channel, "(%f, %f)", vec.x, vec.y);
},
vec2 vec)

POLYMORPHIC_LOGGING_FUNCTION(
void
log,
{
  log(channel, "(%f, %f, %f)", vec.x, vec.y, vec.z);
},
vec3 vec)

POLYMORPHIC_LOGGING_FUNCTION(
void
log,
{
  log(channel, "(%f, %f, %f, %f)", vec.w, vec.x, vec.y, vec.z);
},
vec4 vec)

POLYMORPHIC_LOGGING_FUNCTION(
void
log,
{
  log(channel, "((%f, %f), (%f, %f))", rect.start.x, rect.start.y, rect.end.x, rect.end.y);
},
Rectangle rect)