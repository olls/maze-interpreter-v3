u32
calc_car_radius(u32 cell_spacing, r32 cell_margin)
{
  u32 result = (cell_spacing - (cell_spacing * cell_margin)) * 0.35f;
  return result;
}


void
init_car(GameState *game_state, u64 time_us, Car *car, u32 cell_x, u32 cell_y, V2 direction = DOWN)
{
  car->update_next_frame = false;
  car->dead = false;
  car->value = 0;
  static u32 cars_id = 0;
  car->id = cars_id++;
  car->target_cell_x = cell_x;
  car->target_cell_y = cell_y;
  car->offset = (V2){0, 0};
  car->direction = direction;
  car->pause_left = 0;
  car->unpause_direction = STATIONARY;
  car->updated_cell_type = CELL_NULL;

  V2 pos = (V2){cell_x, cell_y} * game_state->cell_spacing + car->offset;
  car->particle_source = new_particle_source(&(game_state->particles), pos, PS_GROW, time_us);
  car->particle_source->particle_prototype.grow.initial_radius = calc_car_radius(game_state->cell_spacing, game_state->cell_margin);
  car->particle_source->particle_prototype.bitmap = &(game_state->particles.smoke_bitmap);
}


void
move_car(GameState *game_state, Maze *maze, Car *car)
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

      Cell *test_cell = get_cell(maze, (car->target_cell_x + test_direction.x), (car->target_cell_y + test_direction.y));
      if (test_cell &&
          test_cell->type != CELL_WALL &&
          test_cell->type != CELL_NULL)
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


u32
cars_in_direct_neighbourhood(Maze *maze, Cars *cars, Cell *cell)
{
  u32 result = 0;

  Car *test_car;
  CarsIterator iter = {};
  while ((test_car = cars_iterator(cars, &iter)))
  {
    u32 test_x = test_car->target_cell_x;
    u32 test_y = test_car->target_cell_y;

    if (((test_x == cell->x - 1 || test_x == cell->x + 1) && test_y == cell->y) ||
        ((test_y == cell->y - 1 || test_y == cell->y + 1) && test_x == cell->x))
    {
      ++result;
    }
  }

  return result;
}


void
car_cell_interactions(Memory *memory, GameState *game_state, u64 time_us, Maze *maze, Functions *functions, Cars *cars, Car *car)
{
  // TODO: Deal with race cars (conditions) ??

  Cell *current_cell = get_cell(maze, car->target_cell_x, car->target_cell_y);

  switch (current_cell->type)
  {
    case (CELL_NULL):
    {
      log(L_CarsSim, "Null");
      car->direction = STATIONARY;
    } break;

    case (CELL_WALL):
    {
      log(L_CarsSim, "Wall");
      car->direction = STATIONARY;
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

      Car *new_car = get_new_car(memory, cars);
      init_car(game_state, time_us, new_car, car->target_cell_x, car->target_cell_y, RIGHT);
      new_car->value = car->value;
      new_car->direction = RIGHT;
      car->direction = LEFT;
    } break;

    case (CELL_FUNCTION):
    {
      Function *function = functions->hash_table + current_cell->function_index;

      if (function->type == FUNCTION_NULL)
      {
        log(L_CarsSim, "Function '%.2s' not defined, skipping.", current_cell->name);
      }
      else
      {
        log(L_CarsSim, "Function: %s, {type=%d, value=%d}", function->name, (u32)function->type, function->value);

        switch (function->type)
        {
          case (FUNCTION_ASSIGNMENT):
          {
            car->value = function->value;
          } break;
          case (FUNCTION_INCREMENT):
          {
            car->value += function->value;
          } break;
          case (FUNCTION_DECREMENT):
          {
            car->value -= function->value;
          } break;
          case (FUNCTION_MULTIPLY):
          {
            car->value *= function->value;
          } break;
          case (FUNCTION_DIVIDE):
          {
            car->value /= function->value;
          } break;

          case (FUNCTION_LESS):
          case (FUNCTION_LESS_EQUAL):
          case (FUNCTION_EQUAL):
          case (FUNCTION_NOT_EQUAL):
          case (FUNCTION_GREATER_EQUAL):
          case (FUNCTION_GREATER):
          {
            bool condition;
            switch (function->type)
            {
              case (FUNCTION_LESS):
                condition = car->value < function->conditional.value;
                break;
              case (FUNCTION_LESS_EQUAL):
                condition = car->value <= function->conditional.value;
                break;
              case (FUNCTION_EQUAL):
                condition = car->value == function->conditional.value;
                break;
              case (FUNCTION_NOT_EQUAL):
                condition = car->value != function->conditional.value;
                break;
              case (FUNCTION_GREATER_EQUAL):
                condition = car->value >= function->conditional.value;
                break;
              case (FUNCTION_GREATER):
                condition = car->value > function->conditional.value;
                break;
              default: break;
            }

            if (condition)
            {
              car->direction = function->conditional.true_direction;
            }
            else
            {
              if (function->conditional.else_exists)
              {
                car->direction = function->conditional.false_direction;
              }
            }
          } break;
          default: break;
        }

        log(L_CarsSim, "New car value: %d", car->value);
      }
    } break;

    case (CELL_ONCE):
    {
      log(L_CarsSim, "Once");
      car->updated_cell_type = CELL_WALL;
    } break;

    case (CELL_UP_UNLESS_DETECT):
    case (CELL_DOWN_UNLESS_DETECT):
    case (CELL_LEFT_UNLESS_DETECT):
    case (CELL_RIGHT_UNLESS_DETECT):
    {
      log(L_CarsSim, "Unless Detect");
      // TODO: This might need optimising for large numbers of cars (We're looping through cars^2)
      if (cars_in_direct_neighbourhood(maze, cars, current_cell) == 0)
      {
        switch (current_cell->type)
        {
          case (CELL_UP_UNLESS_DETECT):
            car->direction = UP;
            break;
          case (CELL_DOWN_UNLESS_DETECT):
            car->direction = DOWN;
            break;
          case (CELL_LEFT_UNLESS_DETECT):
            car->direction = LEFT;
            break;
          case (CELL_RIGHT_UNLESS_DETECT):
            car->direction = RIGHT;
            break;
          default:
            break;
        }
      }
    } break;

    case (CELL_OUT):
    {
      log(L_CarsSim, "Output");
      printf("%d\n", car->value);
      snprintf(game_state->persistent_str, 256, "%d", car->value);
    } break;

    case (CELL_INP):
    {
      log(L_CarsSim, "Input");
      init_car_input_box(memory, game_state, car->id, car->value, car->target_cell_x, car->target_cell_y);
    } break;

    case (CELL_UP):
    case (CELL_DOWN):
    case (CELL_LEFT):
    case (CELL_RIGHT):
    {
      log(L_CarsSim, "Direction");
      car->direction = get_direction_cell_direction(current_cell->type);
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

    default:
    {
      invalid_code_path;
    } break;
  }
}


void
perform_cars_sim_tick(Memory *memory, GameState *game_state, u64 time_us)
{
  Cars *cars = &(game_state->cars);
  Maze *maze = &(game_state->maze);
  Functions *functions = &(game_state->functions);

  // Car/cell interactions

  CarsIterator iter = {};
  Car *car;

  while ((car = cars_iterator(cars, &iter)))
  {
    if (car->update_next_frame)
    {
      car_cell_interactions(memory, game_state, time_us, maze, functions, cars, car);
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
}


void
move_cars(GameState *game_state)
{
  CarsIterator iter = {};
  Car *car;
  while ((car = cars_iterator(&game_state->cars, &iter)))
  {
    move_car(game_state, &game_state->maze, car);
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