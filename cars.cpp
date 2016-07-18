void
calculate_car_direction(GameState *game_state, Maze *maze, Car *car)
{
  if (car->direction == STATIONARY)
  {
    // They're stuck this way, FOREVER!
  }
  else
  {
    V2 dir_components[] = {UP, DOWN, LEFT, RIGHT};

    V2 directions[4];
    directions[0] = car->direction;
    directions[3] = -car->direction;

    u32 i = 1;
    for (u32 direction_index = 0;
         direction_index < 4;
         direction_index++)
    {
      V2 test_direction = dir_components[direction_index];

      if ((test_direction != directions[0]) &&
          (test_direction != directions[3]))
      {
        directions[i++] = test_direction;
      }
    }

    b32 can_move = false;
    for (u32 direction_index = 0;
         direction_index < 4;
         direction_index++)
    {
      V2 test_direction = directions[direction_index];

      // TODO: Errors if accesses non-existing cell
      Cell *test_cell = get_cell(maze, (car->target_cell_x + test_direction.x), (car->target_cell_y + test_direction.y));
      if (test_cell && test_cell->type != CELL_WALL)
      {
        can_move = true;
        car->direction = test_direction;
        break;
      }
    }

    if (can_move)
    {
      car->offset = -car->direction * game_state->cell_spacing;
      car->target_cell_x += car->direction.x;
      car->target_cell_y += car->direction.y;
    }
  }
}


void
car_cell_interactions(Memory *memory, GameState *game_state, u64 time_us, Maze *maze, Cars *cars, Car *car)
{
  // TODO: Deal with race cars (conditions) ??

  Cell *current_cell = get_cell(maze, car->target_cell_x, car->target_cell_y);

  switch (current_cell->type)
  {
    case (CELL_NULL):
    {
      log(L_CarsSim, "Null");
      invalid_code_path;
    } break;

    case (CELL_WALL):
    {
      log(L_CarsSim, "Wall");
      invalid_code_path;
    } break;

    case (CELL_START):
    {
      log(L_CarsSim, "Start");
    } break;

    case (CELL_PATH):
    {
    } break;

    case (CELL_HOLE):
    {
      log(L_CarsSim, "Hole");
      car->dead = true;
    } break;

    case (CELL_SPLITTER):
    {
      log(L_CarsSim, "Splitter");
      add_car(memory, game_state, time_us, cars, car->target_cell_x, car->target_cell_y, RIGHT);
      car->direction = LEFT;
    } break;

    case (CELL_FUNCTION):
    {
      Function *function = maze->functions + current_cell->function_index;
      log(L_CarsSim, "Function: %s, %d", function->name, (u32)function->type);
    } break;

    case (CELL_ONCE):
    {
      log(L_CarsSim, "Once");
      car->updated_cell_type = CELL_WALL;
    } break;

    case (CELL_SIGNAL):
    {
      log(L_CarsSim, "Signal");
    } break;

    case (CELL_INC):
    {
      log(L_CarsSim, "Increment");
      ++car->value;
    } break;

    case (CELL_DEC):
    {
      log(L_CarsSim, "Decrement");
      --car->value;
    } break;

    case (CELL_UP):
    {
      log(L_CarsSim, "Up");
      car->direction = UP;
    } break;

    case (CELL_DOWN):
    {
      log(L_CarsSim, "Down");
      car->direction = DOWN;
    } break;

    case (CELL_LEFT):
    {
      log(L_CarsSim, "Left");
      car->direction = LEFT;
    } break;

    case (CELL_RIGHT):
    {
      log(L_CarsSim, "Right");
      car->direction = RIGHT;
    } break;

    case (CELL_PAUSE):
    {
      if (car->pause_left != 0)
      {
        --car->pause_left;
      }

      if (car->pause_left == 0)
      {
        if (car->unpause_direction == STATIONARY)
        {
          // Start Pause
          car->pause_left = current_cell->pause;
          car->unpause_direction = car->direction;
          car->direction = STATIONARY;
        }
        else
        {
          // End Pause
          car->direction = car->unpause_direction;
          car->unpause_direction = STATIONARY;
        }
      }
      log(L_CarsSim, "Pause: %d/%d", car->pause_left, current_cell->pause);
    } break;
  }
}


void
update_cars(Memory *memory, GameState *game_state, u64 time_us)
{
  Cars *cars = &(game_state->cars);
  Maze *maze = &(game_state->maze);

  // Car/cell interactions

  CarsIterator iter = {};
  Car *car;

  while ((car = cars_iterator(cars, &iter)))
  {
    if (car->update_next_frame)
    {
      car_cell_interactions(memory, game_state, time_us, maze, cars, car);
    }
  }

  while ((car = cars_iterator(cars, &iter)))
  {
    // NOTE: Necessary to do this loop separate from the previous
    //         loop to set all new cells (including those in new
    //         blocks - which would not have been iterated over in the
    //         same loop) update_next_frame to true.

    car->update_next_frame = true;
  }

  update_dead_cars(cars);

  // Break loop here in case of multiple cars on the same cell

  while ((car = cars_iterator(cars, &iter)))
  {
    if (car->updated_cell_type != CELL_NULL)
    {
      Cell *current_cell = get_cell(maze, car->target_cell_x, car->target_cell_y);
      current_cell->type = car->updated_cell_type;
      car->updated_cell_type = CELL_NULL;
    }
  }

  while ((car = cars_iterator(cars, &iter)))
  {
    calculate_car_direction(game_state, maze, car);
  }
}


void
update_car_position(GameState *game_state, Car *car, u32 last_frame_dt)
{
#if 1
  r32 speed = (last_frame_dt / 1000000.0) * game_state->sim_ticks_per_s * game_state->cell_spacing;
  V2 direction = -unit_vector(car->offset);

  V2 movement = direction * speed;

  if (abs(car->offset.x) >= abs(movement.x))
  {
    car->offset.x += movement.x;
  }
  else
  {
    car->offset.x = 0;
  }

  if (abs(car->offset.y) >= abs(movement.y))
  {
    car->offset.y += movement.y;
  }
  else
  {
    car->offset.y = 0;
  }

#else
  car->offset = (V2){0};
#endif
}


void
annimate_cars(GameState *game_state, u32 last_frame_dt)
{
  CarsIterator iter = {};
  Car *car;
  while ((car = cars_iterator(&game_state->cars, &iter)))
  {
    update_car_position(game_state, car, last_frame_dt);
    car->particle_source->pos = (V2){car->target_cell_x, car->target_cell_y} * game_state->cell_spacing + car->offset;
  }
}