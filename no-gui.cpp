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
main(int argc, char const *argv[])
{
  register_game_logging_channels(GAME_LOGGING_CHANNEL_DEFINITIONS);

  Memory memory;
  memory.total = TOTAL_MEMORY;
  memory.memory = (u8 *)malloc(memory.total);
  memory.used = 0;

  GameState gs;
  GameState *game_state = &gs;

  game_state->init = true;
  game_state->ui.car_inputs = 0;

  game_state->single_step = false;
  game_state->sim_ticks_per_s = 5;

  if (argc > 1)
  {
    game_state->filename = argv[1];
  }
  else
  {
    printf("Error: No Maze filename supplied.\n");
    return 0;
  }

  b32 success = load_maze(&memory, game_state, argc, argv);
  if (!success)
  {
    printf("Error: Couldn't load maze.\n");
    return 0;
  }

  game_state->cars.first_block = 0;
  game_state->cars.free_chain = 0;

  do
  {
    perform_cells_sim_tick(&memory, game_state, &(game_state->maze.tree), 0);
    perform_cars_sim_tick(&memory, game_state, 0);

    move_cars(game_state);

    ++game_state->sim_steps;
  }
  while (game_state->cars.first_block != 0);

  return 0;
}