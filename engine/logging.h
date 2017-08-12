#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) u8(#STRING),


#define ENGINE_LOGGING_CHANNELS_DEFINITIONS(CHANNEL) \
          CHANNEL(L_Main) \
          CHANNEL(L_Bitmap) \
          CHANNEL(L_SVG) \
          CHANNEL(L_XML) \
          CHANNEL(L_XMLTokens) \
          CHANNEL(L_Font) \
          CHANNEL(L_OpenGL) \
          CHANNEL(L_OpenGL_Debug) \
          CHANNEL(L_Layouter) \
          CHANNEL(L_OpenType) \
          CHANNEL(L_Triangulation) \
          CHANNEL(N_ENGINE_LOGGING_CHANNELS)


enum ENGINELoggingChannel
{
  ENGINE_LOGGING_CHANNELS_DEFINITIONS(GENERATE_ENUM)
};

static const u8 *ENGINE_LOGGING_CHANNELS[] = {
  ENGINE_LOGGING_CHANNELS_DEFINITIONS(GENERATE_STRING)
};


// Forward definitions which the game layer must define
enum GAMELoggingChannel : short;

b32
channel_enabled(GAMELoggingChannel);


// Pointer to list of game logging channel strings matching the
//   GAMELoggingChannel enum, this pointer is set in
//   register_game_logging_channels()
static const u8 **GAME_LOGGING_CHANNELS;


#define DEFINE_GAME_LOGGING_CHANNELS(CHANNELS) { \
  CHANNELS(GENERATE_STRING) \
}; \
 \
enum GAMELoggingChannel : short \
{ \
  CHANNELS(GENERATE_ENUM) \
}


// General macro used to generate two functions for
//   ENGINELoggingChannel and GAMELoggingChannel. Also generates
//   not-debug dummy versions.

#ifdef DEBUG
#define POLYMORPHIC_LOGGING_FUNCTION(RETURN_AND_NAME, BODY, ...) \
  RETURN_AND_NAME(ENGINELoggingChannel channel, __VA_ARGS__) \
  { BODY } \
  RETURN_AND_NAME(GAMELoggingChannel channel, __VA_ARGS__) \
  { BODY }
#else
#define POLYMORPHIC_LOGGING_FUNCTION(RETURN_AND_NAME, BODY, ...) \
  RETURN_AND_NAME(ENGINELoggingChannel channel, __VA_ARGS__) \
  { } \
  RETURN_AND_NAME(GAMELoggingChannel channel, __VA_ARGS__) \
  { }
#endif


// Macros used to generate two logging functions for
//   ENGINELoggingChannel and GAMELoggingChannel. It also adds the
//   va_args boilerplate. Also generates not-debug dummy versions.

#ifdef DEBUG
#define _GENERATE_LOGGING_FUNCTION_ENDPOINT(LOGGING_TYPE, RETURN_AND_NAME, BODY, ...) \
  RETURN_AND_NAME(LOGGING_TYPE ## LoggingChannel channel, __VA_ARGS__) \
  { \
    if (channel_enabled(channel)) \
    { \
      u8 buf[1024]; \
      va_list aptr; \
      va_start(aptr, text); \
      vsnprintf((char *)buf, 1024, (const char *)text, aptr); \
      va_end(aptr); \
      \
      const u8 *channel_name = LOGGING_TYPE ## _LOGGING_CHANNELS[channel]; \
      \
      BODY \
    } \
  }
#else
#define _GENERATE_LOGGING_FUNCTION_ENDPOINT(LOGGING_TYPE, RETURN_AND_NAME, BODY, ...) \
  RETURN_AND_NAME(LOGGING_TYPE ## LoggingChannel channel, __VA_ARGS__) \
  { }
#endif

#define POLYMORPHIC_LOGGING_ENDPOINT(RETURN_AND_NAME, BODY, ...) \
  _GENERATE_LOGGING_FUNCTION_ENDPOINT(ENGINE, RETURN_AND_NAME, BODY, __VA_ARGS__) \
  _GENERATE_LOGGING_FUNCTION_ENDPOINT(GAME, RETURN_AND_NAME, BODY, __VA_ARGS__)
