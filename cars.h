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

  V2 direction;

  u32 pause_left;
  V2 unpause_direction;

  CellType updated_cell_type;

  ParticleSource *particle_source;
};