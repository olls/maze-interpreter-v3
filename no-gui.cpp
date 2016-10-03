#include <unistd.h>
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// Game Independent
#include "platform.h"
#include "maths.h"
#include "vectors.h"
#include "render.h"
#include "bitmap.h"

#include "main.h"

#include "logging.cpp"
#include "platform.cpp"
#include "maths.cpp"
#include "vectors.cpp"
#include "render.cpp"
#include "colors.cpp"
#include "bitmap.cpp"

// Game Related
#include "functions.h"
#include "maze.h"
#include "parser.h"
#include "particles.h"
#include "cars-storage.h"
#include "cars.h"
#include "draw.h"
#include "input.h"

#include "maze-interpreter.h"

#include "functions.cpp"
#include "maze.cpp"
#include "parser.cpp"
#include "particles.cpp"
#include "cars-storage.cpp"
#include "cars.cpp"
#include "draw.cpp"
#include "input.cpp"

#include "maze-interpreter.cpp"


int main(int argc, char const *argv[])
{
  Memory memory;
  memory.total = TOTAL_MEMORY;
  memory.memory = (u8 *)malloc(memory.total);
  memory.used = 0;

  GameState gm;
  GameState *game_state = &gm;

  game_state->init = true;

  reset_zoom(game_state);

  game_state->single_step = false;
  game_state->sim_ticks_per_s = 10;

  if (argc > 1)
  {
    parse(&(game_state->maze), &memory, argv[1]);
  }
  else
  {
    parse(&(game_state->maze), &memory, MAZE_FILENAME);
  }

  game_state->cars.first_block = 0;
  game_state->cars.free_chain = 0;

  do
  {
    update_cells(&memory, game_state, &(game_state->maze.tree), 0);
    update_cars(&memory, game_state, 0);

    ++game_state->sim_steps;
  }
  while (game_state->cars.first_block != 0);

  return 0;
}