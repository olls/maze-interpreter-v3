#define DEBUG
// #define DEBUG_BLOCK_COLORS


#include "engine/engine-includes.h"

#include "logging-channels.h"
#include "functions.h"
#include "world-position.h"
#include "particles.h"
#include "cells-storage.h"
#include "cars-storage.h"
#include "cars.h"
#include "parser.h"
#include "ui.h"
#include "cells.h"
#include "serialize.h"
#include "input.h"
#include "opengl-cells-instancing.h"

#include "maze-interpreter.h"

#include "logging-channels.cpp"
#include "functions.cpp"
#include "world-position.cpp"
#include "particles.cpp"
#include "cells-storage.cpp"
#include "cars-storage.cpp"
#include "parser.cpp"
#include "ui.cpp"
#include "cars.cpp"
#include "cells.cpp"
#include "serialize.cpp"
#include "input.cpp"
#include "opengl-cells-instancing.cpp"

#include "maze-interpreter.cpp"


int
main(int argc, const char *argv[])
{
  register_game_logging_channels(GAME_LOGGING_CHANNEL_DEFINITIONS);

  b32 success = start_engine(argc, (const u8 **)argv, update_and_render);
  return !success;
}