#define ALL_LOGGING_CHANNELS(CHANNEL) \
          CHANNEL(L_Main) \
          CHANNEL(L_CarsStorage) \
          CHANNEL(L_CarsSim) \
          CHANNEL(L_CellsStorage) \
          CHANNEL(L_Cells) \
          CHANNEL(L_Parser) \
          CHANNEL(L_Serializer) \
          CHANNEL(L_Particles) \
          CHANNEL(L_UI) \
          CHANNEL(L_Bitmap) \
          CHANNEL(L_SVG) \
          CHANNEL(L_XML) \
          CHANNEL(L_XMLTokens) \
          CHANNEL(L_Font) \
          CHANNEL(L_Vector) \
          CHANNEL(L_Render) \
          CHANNEL(L_OpenGL) \
          CHANNEL(L_CellInstancing) \
          CHANNEL(L_Layouter) \
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
