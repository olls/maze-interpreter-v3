#define GAME_LOGGING_CHANNELS(CHANNEL) \
          CHANNEL(L_CarsStorage) \
          CHANNEL(L_CarsSim) \
          CHANNEL(L_CellsStorage) \
          CHANNEL(L_Cells) \
          CHANNEL(L_Parser) \
          CHANNEL(L_Serializer) \
          CHANNEL(L_Particles) \
          CHANNEL(L_UI) \
          CHANNEL(L_Render) \
          CHANNEL(L_CellInstancing) \
          CHANNEL(L_GameLoop) \
          CHANNEL(N_GAME_LOGGING_CHANNELS)


const u8 *GAME_LOGGING_CHANNEL_DEFINITIONS[] = DEFINE_GAME_LOGGING_CHANNELS(GAME_LOGGING_CHANNELS);