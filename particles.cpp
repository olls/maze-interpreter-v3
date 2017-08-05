ParticleSource *
new_particle_source(Particles *particles, WorldSpace pos, ParticleType type, u64 time_us)
{
  ParticleSource *result = 0;

  result = particles->sources + particles->next_free_particle_source++;
  particles->next_free_particle_source %= MAX_PARTICLE_SOURCES;

  *result = (ParticleSource){};

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
      result->spawn_rate = 10;
      result->ttl = -1;

      result->particle_prototype.ttl = 2000000;
      result->particle_prototype.fade_out = true;
      result->particle_prototype.speed = 40000;
      result->particle_prototype.grow.initial_radius = 0;
    } break;
  }

  log(L_Particles, u8("New particle system"));

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
            new_particle->source_pos = source->pos;
            new_particle->offset = (vec2){0};

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
                new_particle->color = (vec4){1, 1, 1, 1};
                new_particle->hue = rand() % 360;
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
render_particles(Particles *particles, RenderWindow *render_window)
{
  for (u32 particle_index = 0;
       particle_index < MAX_PARTICLES;
       ++particle_index)
  {
    Particle *particle = particles->particles + particle_index;

    if (particle->t0)
    {
      WorldSpace particle_pos = particle->source_pos;
      particle_pos.offset += particle->offset;
      re_form_world_coord(&particle_pos);

      vec2 normalised_pos = world_coord_to_render_window_coord(render_window, particle_pos);

      switch (particle->type)
      {
        case PS_CIRCLE:
        {
          glPushMatrix();
            glColor3f(particle->color.r, particle->color.g, particle->color.b);
            glTranslatef(normalised_pos.x, normalised_pos.y, 0);
            glScalef(0.1, 0.1, 0.1);
            draw_circle();
          glPopMatrix();
        } break;
        case PS_GROW:
        {
          vec2 bitmap_size = Vec2(particle->bitmap->file->width,
                                  particle->bitmap->file->height);

          // BlitBitmapOptions blit_opts;
          // get_default_blit_bitmap_options(&blit_opts);

          // blit_opts.color_multiplier = particle->color;
          // blit_opts.hue_shift = particle->hue;
          // blit_opts.interpolation = true;
          // draw_bitmap(particle->bitmap, normalised_pos - (bitmap_size * .5f), &blit_opts);
        } break;
      }
    }
  }
}