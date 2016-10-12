#include <stdarg.h>
#include <stdio.h>

#define ALL_LOGGING_CHANNELS(CHANNEL) \
          CHANNEL(L_Main) \
          CHANNEL(L_CarsStorage) \
          CHANNEL(L_CarsSim) \
          CHANNEL(L_CellsStorage) \
          CHANNEL(L_Cells) \
          CHANNEL(L_Parser) \
          CHANNEL(L_Particles) \
          CHANNEL(L_Bitmap) \
          CHANNEL(L_Font) \
          CHANNEL(L_Render) \
          CHANNEL(L_GameLoop) \
          CHANNEL(N_LOGGING_CHANNELS)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum LoggingChannel
{
  ALL_LOGGING_CHANNELS(GENERATE_ENUM)
};

static const char *LOGGING_CHANNELS[] = {
  ALL_LOGGING_CHANNELS(GENERATE_STRING)
};


b32
channel_enabled(LoggingChannel channel)
{
  switch(channel)
  {
    case L_Main:
    case L_CarsStorage:
    case L_CarsSim:
    case L_CellsStorage:
    case L_Cells:
    case L_Parser:
    case L_Particles:
    case L_Bitmap:
    case L_Font:
    case L_Render:
    case L_GameLoop:
    {
      return false;
    } break;

    default:
    {
      return true;
    } break;
  }
}


void
log_s(LoggingChannel channel, const char *text, ...)
{
  if (channel_enabled(channel))
  {
    char buf[256];
    va_list aptr;
    va_start(aptr, text);
    vsnprintf(buf, 256, text, aptr);
    va_end(aptr);

    printf("%s", buf);
  }
}


void
log(LoggingChannel channel, const char *text, ...)
{
  if (channel_enabled(channel))
  {
    char buf[256];
    va_list aptr;
    va_start(aptr, text);
    vsnprintf(buf, 256, text, aptr);
    va_end(aptr);

    printf("\e[01;3%dm%s\e[0m -> %s\n", channel % 8, LOGGING_CHANNELS[channel], buf);
  }
}


void
log_d(LoggingChannel channel, V2 direction)
{
  char s[16] = {};

  if (direction.x == 0 && direction.y == -1)
  {
    strcpy(s, "UP");
  }
  else if (direction.x == 0 && direction.y == 1)
  {
    strcpy(s, "DOWN");
  }
  else if (direction.x == -1 && direction.y == 0)
  {
    strcpy(s, "LEFT");
  }
  else if (direction.x == 1 && direction.y == 0)
  {
    strcpy(s, "RIGHT");
  }
  else if (direction.x == 0 && direction.y == 0)
  {
    strcpy(s, "STATIONARY");
  }

  if (s[0])
  {
    log_s(channel, s);
  }
}


void
log(LoggingChannel channel, V4 vec)
{
  log(channel, "(%f, %f, %f, %f)", vec.w, vec.x, vec.y, vec.z);
}

void
log(LoggingChannel channel, V2 vec)
{
  log(channel, "(%f, %f)", vec.x, vec.y);
}

void
log(LoggingChannel channel, V3 vec)
{
  log(channel, "(%f, %f, %f)", vec.x, vec.y, vec.z);
}

void
log(LoggingChannel channel, Rectangle rect)
{
  log(channel, "((%f, %f), (%f, %f))", rect.start.x, rect.start.y, rect.end.x, rect.end.y);
}