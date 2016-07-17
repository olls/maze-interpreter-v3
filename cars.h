const u32 CARS_PER_BLOCK = 512;

// Car storage needs to:
// - Add car
// - Remove car (Remove all cars with marker)
// - Loop through all cars
//
// Therefore linked list of blocks, with free list for deallocated blocks.
// - When adding a car:
//   - Loop through blocks until found one with space
//   - If all blocks full
//     - Try get block from free_chain
//       - Else allocate new block
//     - Add to start of chain
// - When removing a car:
//   - Set dead flag
//   - update_dead_cars routine:
//     - For all cars with dead flag:
//       - If no cars left in block
//         - Add to start of free_chain
//         - Re-link previous block to next block


struct Car
{
  b32 update_next_frame;
  b32 dead;

  u32 id; // NOTE: For debugging purposes only

  s32 value;

  u32 target_cell_x;
  u32 target_cell_y;

  // NOTE: Used for sub-cell positioning, in world space.
  V2 offset;

  Direction direction;

  u32 pause_left;
  Direction unpause_direction;

  CellType updated_cell_type;

  ParticleSource *particle_source;
};


struct CarsBlock
{
  Car cars[CARS_PER_BLOCK];
  u32 next_free_in_block;
  CarsBlock *next_block;

#ifdef DEBUG_BLOCK_COLORS
  V4 c;
#endif
};


struct Cars
{
  CarsBlock *first_block;
  CarsBlock *free_chain;
};