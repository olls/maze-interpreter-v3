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
      printf("Getting car block from free chain\n");
    }
    else
    {
      block = push_struct(memory, CarsBlock);
      printf("Allocating new car block\n");
    }

    block->next_free_in_block = 0;
    block->next_block = 0;
    block->prev_block = 0;

    if (cars->first_block)
    {
      CarsBlock *second_block = cars->first_block;

      second_block->prev_block = block;
      block->next_block = second_block;
    }

    cars->first_block = block;
  }

  Car *car = block->cars + block->next_free_in_block++;

  car->dead = false;
  car->value = 0;
  car->cell_x = cell_x;
  car->cell_y = cell_y;
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


void
update_dead_cars(Cars *cars)
{
  CarsBlock *cars_block = cars->first_block;
  u32 first_car_not_checked_in_block = 0;

  while (cars_block)
  {
    Car *car = cars_block->cars + first_car_not_checked_in_block;

    if (car->dead)
    {
      printf("Deleting car\n");
      rm_car(cars_block, first_car_not_checked_in_block);

      if (cars_block->next_free_in_block == 0)
      {
        printf("Deallocating car block\n");

        CarsBlock *next_block = cars_block->next_block;

        // Reconnect previous and next block's links
        if (cars_block->next_block)
        {
          cars_block->next_block->prev_block = cars_block->prev_block;
        }
        if (cars_block->prev_block)
        {
          cars_block->prev_block->next_block = cars_block->next_block;
        }

        // Re-link start if deallocated block was first block
        if (cars->first_block == cars_block)
        {
          cars->first_block = cars_block->next_block;
        }

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
      cars_block = cars_block->next_block;
    }
  }
}


void
car_movements(Maze *maze, Car *car)
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
      switch (car->direction)
      {
        case UP:
        {
          --car->target_cell_y;
        } break;

        case DOWN:
        {
          ++car->target_cell_y;
        } break;

        case LEFT:
        {
          --car->target_cell_x;
        } break;

        case RIGHT:
        {
          ++car->target_cell_x;
        } break;

        case STATIONARY:
        {
        } break;

        default:
        {
          invalid_code_path;
        }
      }
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
      printf("Null\n");
      invalid_code_path;
    } break;

    case (CELL_WALL):
    {
      printf("Wall\n");
      invalid_code_path;
    } break;

    case (CELL_START):
    {
      printf("Start\n");
    } break;

    case (CELL_PATH):
    {
    } break;

    case (CELL_HOLE):
    {
      printf("Hole\n");
      car->dead = true;
    } break;

    case (CELL_SPLITTER):
    {
      printf("Splitter\n");
      add_car(memory, cars, car->target_cell_x, car->target_cell_y, RIGHT);
      car->direction = LEFT;
    } break;

    case (CELL_FUNCTION):
    {
      Function *function = maze->functions + current_cell->function_index;
      printf("Function: %s, %d\n", function->name, (u32)function->type);
    } break;

    case (CELL_ONCE):
    {
      printf("Once\n");
      car->updated_cell_type = CELL_WALL;
    } break;

    case (CELL_SIGNAL):
    {
      printf("Signal\n");
    } break;

    case (CELL_INC):
    {
      printf("Increment\n");
      ++car->value;
    } break;

    case (CELL_DEC):
    {
      printf("Decrement\n");
      --car->value;
    } break;

    case (CELL_UP):
    {
      printf("Up\n");
      car->direction = UP;
    } break;

    case (CELL_DOWN):
    {
      printf("Down\n");
      car->direction = DOWN;
    } break;

    case (CELL_LEFT):
    {
      printf("Left\n");
      car->direction = LEFT;
    } break;

    case (CELL_RIGHT):
    {
      printf("Right\n");
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
      printf("Pause: %d/%d\n", car->pause_left, current_cell->pause);
    } break;
  }
}


void
re_canonicalise_car_pos(GameState *game_state, Car *car)
{
  if (abs(car->offset.x) >= game_state->cell_spacing)
  {
    car->cell_x += (s32)car->offset.x / (s32)game_state->cell_spacing;
    car->offset.x = (s32)car->offset.x % (s32)game_state->cell_spacing;
  }
  if (abs(car->offset.y) >= game_state->cell_spacing)
  {
    car->cell_y += (s32)car->offset.y / (s32)game_state->cell_spacing;
    car->offset.y = (s32)car->offset.y % (s32)game_state->cell_spacing;
  }
}


void
update_car_position(GameState *game_state, Car *car)
{
#if 1
  u32 total_cell_dx = car->target_cell_x - car->cell_x;
  u32 total_cell_dy = car->target_cell_y - car->cell_y;

  V2 target_direction = unit_vector((V2){total_cell_dx, total_cell_dy});

  // TODO: Calculate speed from FPS, CAR_TICKS_PER_S ...
  r32 speed = 25000;
  car->offset += target_direction *speed;
  re_canonicalise_car_pos(game_state, car);

  // TODO: Correct if overshot

#else
  // Disable animations

  car->cell_x = car->target_cell_x;
  car->cell_y = car->target_cell_y;
  car->offset = (V2){0};
#endif
}


void
update_cars(Memory *memory, GameState *game_state, u64 time_us)
{
  Cars *cars = &(game_state->cars);
  Maze *maze = &(game_state->maze);
  // TODO: Check this actually working!
  b32 car_tick = false;
  if (time_us >= game_state->last_car_update + (seconds_in_u(1) / CAR_TICKS_PER_S))
  {
    game_state->last_car_update = time_us;
    car_tick = true;
  }

  // Car/cell interactions

  CarsBlock *cars_block;

  if (car_tick)
  {

    cars_block = cars->first_block;
    while (cars_block != 0)
    {
      // NOTE: Store n_cars so cars added within the loop aren't looped over.
      u32 n_cars = cars_block->next_free_in_block;
      for (u32 car_index = 0;
           car_index < n_cars;
           ++car_index)
      {
        Car *car = cars_block->cars + car_index;

        car_cell_interactions(memory, maze, cars, car);
      }
      cars_block = cars_block->next_block;
    }

    // Update cell types
    cars_block = cars->first_block;
    while (cars_block != 0)
    {
      for (u32 car_index = 0;
           car_index < cars_block->next_free_in_block;
           ++car_index)
      {
        Car *car = cars_block->cars + car_index;

        if (car->updated_cell_type != CELL_NULL)
        {
          Cell *current_cell = get_cell(maze, car->target_cell_x, car->target_cell_y);
          current_cell->type = car->updated_cell_type;
          car->updated_cell_type = CELL_NULL;
        }
      }
      cars_block = cars_block->next_block;
    }
  }

  // Car deletion / movement

  if (car_tick)
  {
    cars_block = cars->first_block;
    while (cars_block != 0)
    {
      for (u32 car_index = 0;
           car_index < cars_block->next_free_in_block;
           ++car_index)
      {
        Car *car = cars_block->cars + car_index;

        update_car_position(game_state, car);
        car_movements(maze, car);
      }
      cars_block = cars_block->next_block;
    }

    update_dead_cars(cars);
  }

  cars_block = cars->first_block;
  while (cars_block != 0)
  {
    for (u32 car_index = 0;
         car_index < cars_block->next_free_in_block;
         ++car_index)
    {
      Car *car = cars_block->cars + car_index;

      update_car_position(game_state, car);
    }
    cars_block = cars_block->next_block;
  }
}