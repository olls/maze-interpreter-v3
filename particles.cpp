ParticleSource *
new_particle_source(Particles *particles, V2 pos, ParticleType type, u64 time_us)
{
  ParticleSource *result = 0;

  result = particles->sources + particles->next_free_particle_source++;
  particles->next_free_particle_source %= MAX_PARTICLE_SOURCES;

  result->pos = pos;
  result->t0 = time_us;
  result->last_spawn = time_us;
  result->particle_prototype.type = type;

  // NOTE: Default values for source and particles, can be customised
  //         by calling code after return from function.
  switch (type)
  {
    case (PS_CIRCLE):
    {
      result->spawn_rate = 10;
      result->ttl = -1;
      result->particle_prototype.ttl = 1000000;
    } break;
    case (PS_GROW):
    {
      result->spawn_rate = 100;
      result->ttl = -1;

      result->particle_prototype.ttl = 200000;
      result->particle_prototype.fade_out = true;
      result->particle_prototype.speed = 200000;
      result->particle_prototype.grow.initial_radius = 0;
    } break;
  }

  log(L_Particles, "New particle system");

  return result;
}


Particle *
create_particle(Particles *particles)
{
  Particle *result = particles->particles + particles->next_particle_to_use++;
  particles->next_particle_to_use %= MAX_PARTICLES;

  return result;
}


void
step_particles(Particles *particles, u64 time_us)
{
  for (u32 particle_source_index = 0;
       particle_source_index < MAX_PARTICLE_SOURCES;
       ++particle_source_index)
  {
    ParticleSource *source = particles->sources + particle_source_index;

    if (source->t0 != 0)
    {
      if (time_us >= source->t0 + source->ttl)
      {
        source->t0 = 0;
      }
      else
      {
        u32 n_spawn = (time_us - source->last_spawn) * (source->spawn_rate / 1000000.0);
        if (n_spawn)
        {
          source->last_spawn = time_us;

          for (u32 i = 0;
               i < n_spawn;
               ++i)
          {
            Particle *new_particle = create_particle(particles);
            *new_particle = source->particle_prototype;

            new_particle->t0 = time_us;
            new_particle->pos = source->pos;
            new_particle->offset = (V2){0};

            // NOTE: Initial setup of particles
            switch (new_particle->type)
            {
              case PS_CIRCLE:
              {
                new_particle->color = get_color(new_particle - particles->particles);
                new_particle->speed = 1000000 + ((rand() % 500000) - 25000);
                new_particle->circle.radius = 30000 + ((rand() % 15000) - 7500);
              } break;
              case PS_GROW:
              {
                new_particle->color = get_color(new_particle - particles->particles);
                new_particle->grow.direction = 2 * M_PI * ((r32)(rand() % 360) / 360.0);
              } break;
            }
          }
        }
      }
    }
  }

  for (u32 particle_index = 0;
       particle_index < MAX_PARTICLES;
       ++particle_index)
  {
    Particle *particle = particles->particles + particle_index;

    if (particle->t0)
    {
      if (time_us >= particle->t0 + particle->ttl)
      {
        particle->t0 = 0;
      }
      else
      {
        u32 age = time_us - particle->t0;

        if (particle->fade_out)
        {
          particle->color.a = 1.0 - ((r32)age / (r32)particle->ttl);
        }

        switch (particle->type)
        {
          case PS_CIRCLE:
          {
            particle->offset.x = sin((2.0*M_PI) * (age / particle->speed)) * particle->circle.radius;
            particle->offset.y = cos((2.0*M_PI) * (age / particle->speed)) * particle->circle.radius;
          } break;
          case PS_GROW:
          {
            // NOTE: Speed is in world per second here
            particle->offset.x = (particle->grow.initial_radius + ((r32)age / 1000000) * (r32)particle->speed) * cos(particle->grow.direction);
            particle->offset.y = (particle->grow.initial_radius + ((r32)age / 1000000) * (r32)particle->speed) * sin(particle->grow.direction);
          } break;
        }
      }
    }
  }
}


void
render_particles(Particles *particles, FrameBuffer *frame_buffer, RenderBasis *render_basis)
{
  for (u32 particle_index = 0;
       particle_index < MAX_PARTICLES;
       ++particle_index)
  {
    Particle *particle = particles->particles + particle_index;

    if (particle->t0)
    {
      V2 pos = particle->pos + particle->offset;

      switch (particle->type)
      {
        case PS_CIRCLE:
        {
          V2 screen_pos = transform_coord(render_basis, pos);
          V2 bitmap_size = {particles->particle_image.file->biWidth, particles->particle_image.file->biHeight};
          blit_bitmap(frame_buffer, &particles->particle_image, screen_pos - (bitmap_size * .5));
        } break;
        case PS_GROW:
        {
          u32 pixel_size = 4000;

          r32 top_y = pos.y - pixel_size * 1.5;
          r32 bottom_y = pos.y + pixel_size * 1.5;
          r32 left_x = pos.x - pixel_size * 1.5;
          r32 right_x = pos.x + pixel_size * 1.5;

          draw_box(frame_buffer, render_basis, (Rectangle){(V2){pos.x - pixel_size*.5, top_y}, (V2){pos.x + pixel_size*.5, bottom_y}}, particle->color);
          draw_box(frame_buffer, render_basis, (Rectangle){(V2){left_x, pos.y - pixel_size*.5}, (V2){right_x, pos.y + pixel_size*.5}}, particle->color);
        } break;
      }
    }
  }
}