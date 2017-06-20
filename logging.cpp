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
    case L_Serializer:
    case L_Particles:
    case L_UI:
    case L_Bitmap:
    case L_SVG:
    case L_XML:
    case L_XMLTokens:
    case L_Font:
    case L_Vector:
    case L_Render:
    case L_RenderQueue:
    // case L_OpenGL:
    case L_Layouter:
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


#ifdef DEBUG


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
log_ind(LoggingChannel channel, u32 n, const char *text, ...)
{
  if (channel_enabled(channel))
  {
    char buf[256];
    va_list aptr;
    va_start(aptr, text);
    vsnprintf(buf, 256, text, aptr);
    va_end(aptr);

    printf("\e[01;3%dm%s\e[0m -> %*s%s\n", channel % 8, LOGGING_CHANNELS[channel], n, "", buf);
  }
}


void
log_d(LoggingChannel channel, V2 direction)
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


#else


void
log_s(LoggingChannel channel, const char *text, ...)
{}

void
log(LoggingChannel channel, const char *text, ...)
{}

void
log_ind(LoggingChannel channel, u32 n, const char *text, ...)
{}

void
log_d(LoggingChannel channel, V2 direction)
{}

void
log(LoggingChannel channel, V4 vec)
{}

void
log(LoggingChannel channel, V2 vec)
{}

void
log(LoggingChannel channel, V3 vec)
{}

void
log(LoggingChannel channel, Rectangle rect)
{}


#endif