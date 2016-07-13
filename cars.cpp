Car *
add_car(Memory *memory, Cars *cars, u32 cell_x, u32 cell_y, Direction direction = DOWN)
{
  CarsBlock *block = cars->first_block;
  while (block && block->next_free_in_block == CARS_PER_BLOCK)
  {
    block = block->next_block;
  }

  if (!block)
  {
    // No blocks left
    if (cars->free_chain)
    {
      block = cars->free_chain;
      cars->free_chain = block->next_block;
      log(L_CarsStorage, "Getting car block from free chain");
    }
    else
    {
      block = push_struct(memory, CarsBlock);

#ifdef DEBUG_BLOCK_COLORS
      static u32 C = 0;
      static V4 CS[] = {(V4){1, 0.0, 0.0, 0.0},
                        (V4){1, 0.0, 0.0, 0.5},
                        (V4){1, 0.0, 0.0, 1.0},
                        (V4){1, 0.0, 0.5, 0.0},
                        (V4){1, 0.0, 0.5, 0.5},
                        (V4){1, 0.0, 0.5, 1.0},
                        (V4){1, 0.0, 1.0, 0.0},
                        (V4){1, 0.0, 1.0, 0.5},
                        (V4){1, 0.0, 1.0, 1.0},
                        (V4){1, 0.5, 0.0, 0.0},
                        (V4){1, 0.5, 0.0, 0.5},
                        (V4){1, 0.5, 0.0, 1.0},
                        (V4){1, 0.5, 0.5, 0.0},
                        (V4){1, 0.5, 0.5, 0.5},
                        (V4){1, 0.5, 0.5, 1.0},
                        (V4){1, 0.5, 1.0, 0.0},
                        (V4){1, 0.5, 1.0, 0.5},
                        (V4){1, 0.5, 1.0, 1.0},
                        (V4){1, 1.0, 0.0, 0.0},
                        (V4){1, 1.0, 0.0, 0.5},
                        (V4){1, 1.0, 0.0, 1.0},
                        (V4){1, 1.0, 0.5, 0.0},
                        (V4){1, 1.0, 0.5, 0.5},
                        (V4){1, 1.0, 0.5, 1.0},
                        (V4){1, 1.0, 1.0, 0.0},
                        (V4){1, 1.0, 1.0, 0.5},
                        (V4){1, 1.0, 1.0, 1.0}};

      block->c = CS[++C];
      C %= 8;
#endif

      log(L_CarsStorage, "Allocating new car block");
    }

    block->next_free_in_block = 0;

    block->next_block = cars->first_block;
    cars->first_block = block;
  }

  static u32 cars_id = 0;
  Car *car = block->cars + block->next_free_in_block++;

  car->update_next_frame = false;
  car->dead = false;
  car->value = 0;
  car->id = cars_id++;
  car->target_cell_x = cell_x;
  car->target_cell_y = cell_y;
  car->offset = (V2){0, 0};
  car->direction = direction;
  car->pause_left = 0;
  car->unpause_direction = STATIONARY;
  car->updated_cell_type = CELL_NULL;

  return car;
}


void
rm_car(CarsBlock *block, u32 index_in_block)
{
  --block->next_free_in_block;
  if (index_in_block != block->next_free_in_block)
  {
    block->cars[index_in_block] = block->cars[block->next_free_in_block];
  }
}


void delete_all_cars(Cars *cars)
{
  CarsBlock *block = cars->first_block;
  if (block)
  {
    while (block->next_block)
    {
      block = block->next_block;
    }
    block->next_block = cars->free_chain;
    cars->free_chain = cars->first_block;
    cars->first_block = 0;
  }
}


void
update_dead_cars(Cars *cars)
{
  CarsBlock *cars_block = cars->first_block;
  CarsBlock *prev_block = 0;

  u32 first_car_not_checked_in_block = 0;

  while (cars_block)
  {
    Car *car = cars_block->cars + first_car_not_checked_in_block;

    if (car->dead)
    {
      log(L_CarsStorage, "Deleting car");
      rm_car(cars_block, first_car_not_checked_in_block);

      if (cars_block->next_free_in_block == 0)
      {
        log(L_CarsStorage, "Deallocating car block");

        // Reconnect previous block's link
        if (cars->first_block == cars_block)
        {
          cars->first_block = cars_block->next_block;
        }
        else
        {
          prev_block->next_block = cars_block->next_block;
        }

        CarsBlock *next_block = cars_block->next_block;

        cars_block->next_block = cars->free_chain;
        cars->free_chain = cars_block;

        first_car_not_checked_in_block = 0;
        cars_block = next_block;
        continue;
      }

    }
    else
    {
      ++first_car_not_checked_in_block;
    }

    if (first_car_not_checked_in_block >= cars_block->next_free_in_block)
    {
      first_car_not_checked_in_block = 0;
      prev_block = cars_block;
      cars_block = cars_block->next_block;
    }
  }
}


void
change_car_pos(GameState *game_state, Car *car, V2 d)
{
  car->offset = -d * game_state->cell_spacing;
  car->target_cell_x += d.x;
  car->target_cell_y += d.y;
}


void
calculate_car_direction(GameState *game_state, Maze *maze, Car *car)
{
  if (car->direction == STATIONARY)
  {
    // They're stuck this way, FOREVER!
  }
  else
  {
    Direction directions[4];
    directions[0] = car->direction;
    directions[3] = reverse(car->direction);

    u32 i = 1;
    for (u32 direction = 0;
         direction < 4;
         direction++)
    {
      if ((direction != directions[0]) &&
          (direction != directions[3]))
      {
        directions[i++] = (Direction)direction;
      }
    }

    V2 dir_components[] = {{ 0, -1}, {0, 1},
                           {-1,  0}, {1, 0}};

    b32 can_move = false;
    for (u32 direction_index = 0;
         direction_index < 4;
         direction_index++)
    {
      Direction test_direction = directions[direction_index];
      V2 test_comp = dir_components[test_direction];

      // TODO: Errors if accesses non-existing cell
      Cell *test_cell = get_cell(maze, (car->target_cell_x + test_comp.x), (car->target_cell_y + test_comp.y));
      if (test_cell)
      {
        if (test_cell->type != CELL_WALL)
        {
          can_move = true;
          car->direction = test_direction;
          break;
        }
      }
    }

    if (can_move)
    {
      V2 d_car_pos = {0};

      switch (car->direction)
      {
        case UP:
        {
          --d_car_pos.y;
        } break;

        case DOWN:
        {
          ++d_car_pos.y;
        } break;

        case LEFT:
        {
          --d_car_pos.x;
        } break;

        case RIGHT:
        {
          ++d_car_pos.x;
        } break;

        case STATIONARY:
        {
        } break;

        default:
        {
          invalid_code_path;
        }
      }

      change_car_pos(game_state, car, d_car_pos);
    }
  }
}


void
car_cell_interactions(Memory *memory, Maze *maze, Cars *cars, Car *car)
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
      add_car(memory, cars, car->target_cell_x, car->target_cell_y, RIGHT);
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
update_car_position(GameState *game_state, Car *car)
{
#if 1
  // TODO: Why this isn't fast enough?
  r32 speed = (1.0f / FPS) * game_state->sim_ticks_per_s * game_state->cell_spacing;
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


struct CarsIterator
{
  CarsBlock *cars_block;
  u32 car_index;
};


Car *
cars_iterator(Cars *cars, CarsIterator *iterator)
{
  Car *result = 0;

  // Check if this is a new iterator
  if (iterator->cars_block == 0)
  {
    iterator->cars_block = cars->first_block;
    iterator->car_index = 0;
  }

  if (iterator->cars_block &&
      iterator->car_index >= iterator->cars_block->next_free_in_block)
  {
    iterator->cars_block = iterator->cars_block->next_block;
    iterator->car_index = 0;
  }

  // Check we haven't reached the end
  if (iterator->cars_block)
  {
    result = iterator->cars_block->cars + iterator->car_index;
    ++iterator->car_index;
  }

  return result;
}


void
update_cars(Memory *memory, GameState *game_state)
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
      car_cell_interactions(memory, maze, cars, car);
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
annimate_cars(GameState *game_state)
{
  CarsIterator iter = {};
  Car *car;
  while ((car = cars_iterator(&game_state->cars, &iter)))
  {
    update_car_position(game_state, car);
  }
}