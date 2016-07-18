#define ALL_LOGGING_CHANNELS(CHANNEL) \
          CHANNEL(L_Main) \
          CHANNEL(L_CarsStorage) \
          CHANNEL(L_CarsSim) \
          CHANNEL(L_Parser) \
          CHANNEL(L_Particles) \
          CHANNEL(L_Bitmap) \
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
    case L_CarsSim:
    case L_CarsStorage:
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