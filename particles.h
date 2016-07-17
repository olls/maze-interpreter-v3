const u32 MAX_PARTICLE_SOURCES = 512;
const u32 MAX_PARTICLES = 512;


enum ParticleType
{
  PS_CIRCLE,
  PS_GROW
};


struct ParticleSource
{
  V2 pos;
  ParticleType type;
  r32 spawn_rate;
  u64 last_spawn;
  u64 t0;
  u32 ttl;
  u32 particle_ttl;
};


struct Particle
{
  u64 t0;
  u32 ttl;
  V2 pos;
  V2 offset;
  ParticleType type;
  r32 speed;
  V4 color;
  union
  {
    struct
    {
      u32 radius;
    } circle;
    struct
    {
      r32 direction;
    } grow;
  };
};


struct Particles
{
  ParticleSource sources[MAX_PARTICLE_SOURCES];
  u32 next_free_particle_source;

  Particle particles[MAX_PARTICLES];
  u32 next_particle_to_use;

  Bitmap particle_image;
};