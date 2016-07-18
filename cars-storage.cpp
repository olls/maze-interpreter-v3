Car *
get_new_car(Memory *memory, Cars *cars)
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
      block->c = get_color(++C);
      C %= 27;
#endif

      log(L_CarsStorage, "Allocating new car block");
    }

    block->next_free_in_block = 0;

    block->next_block = cars->first_block;
    cars->first_block = block;
  }

  Car *result = block->cars + block->next_free_in_block++;
  return result;
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
rm_car(CarsBlock *block, u32 index_in_block)
{
  Car *car = block->cars + index_in_block;
  car->particle_source->t0 = 0;

  --block->next_free_in_block;
  if (index_in_block != block->next_free_in_block)
  {
    *car = block->cars[block->next_free_in_block];
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
