Car *
add_car(Cars *cars, u32 cell_x, u32 cell_y, Direction direction = UP)
{
  assert(cars->next_free != MAX_CARS);

  Car *car = cars->cars + cars->next_free;
  ++cars->next_free;

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
rm_car(Cars *cars, u32 car_index)
{
  --cars->next_free;
  if (car_index != cars->next_free)
  {
    cars->cars[car_index] = cars->cars[cars->next_free];
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

    // TODO: Maybe grab the cells in chunks, so we don't have to get the
    //       cells multiple times...
    //       Or cache them in maze.cpp

    V2 dir_components[] = {{ 0, -1}, {0, 1},
                           {-1,  0}, {1, 0}};

    b32 can_move = false;
    for (u32 direction_index = 0;
         direction_index < 4;
         direction_index++)
    {
      Direction test_direction = directions[direction_index];
      V2 test_comp = dir_components[test_direction];

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
car_cell_interactions(Maze *maze, Cars *cars, Car *car)
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
      add_car(cars, car->target_cell_x, car->target_cell_y, RIGHT);
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
  u32 total_cell_dx = car->target_cell_x - car->cell_x;
  u32 total_cell_dy = car->target_cell_y - car->cell_y;

  V2 target_direction = unit_vector((V2){total_cell_dx, total_cell_dy});

  // TODO: Calculate speed from FPS, CAR_TICKS_PER_S ...
  r32 speed = 25000;
  car->offset += target_direction *speed;
  re_canonicalise_car_pos(game_state, car);

  // TODO: Correct if overshot
}


void
update_cars(GameState *game_state, u64 time_us)
{
  Cars *cars = &(game_state->cars);
  Maze *maze = &(game_state->maze);

  b32 car_tick = false;
  if (time_us >= game_state->last_car_update + (seconds_in_u(1) / CAR_TICKS_PER_S))
  {
    game_state->last_car_update = time_us;
    car_tick = true;
  }

  // Car/cell interactions

  if (car_tick)
  {
    // NOTE: Store n_cars so cars added within the loop aren't looped over.
    u32 n_cars = cars->next_free;
    for (u32 car_index = 0;
         car_index < n_cars;
         ++car_index)
    {
      Car *car = cars->cars + car_index;
      car_cell_interactions(maze, cars, car);
    }

    // Update cell types
    for (u32 car_index = 0;
         car_index < cars->next_free;
         ++car_index)
    {
      Car *car = cars->cars + car_index;

      if (car->updated_cell_type != CELL_NULL)
      {
        Cell *current_cell = get_cell(maze, car->target_cell_x, car->target_cell_y);
        current_cell->type = car->updated_cell_type;
        car->updated_cell_type = CELL_NULL;
      }
    }
  }

  // Car deletion / movement

  if (car_tick)
  {
    for (u32 car_index = 0;
         car_index < cars->next_free;
         ++car_index)
    {
      Car *car = cars->cars + car_index;

      // Delete cars
      while (car->dead && cars->next_free != car_index)
      {
        rm_car(cars, car_index);
      }
      if (cars->next_free == car_index)
      {
        break;
      }

      car_movements(maze, car);
    }
  }

  for (u32 car_index = 0;
       car_index < cars->next_free;
       ++car_index)
  {
    Car *car = cars->cars + car_index;
    update_car_position(game_state, car);
  }
}