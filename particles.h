const u32 MAX_PARTICLE_SOURCES = 512;
const u32 MAX_PARTICLES = 8192;


enum ParticleType
{
  PS_CIRCLE,
  PS_GROW
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
  r32 hue;
  b32 fade_out;
  union
  {
    struct
    {
      u32 radius;
    } circle;
    struct
    {
      u32 initial_radius;
      r32 direction;
    } grow;
  };
};


struct ParticleSource
{
  V2 pos;
  r32 spawn_rate;
  u64 last_spawn;
  u64 t0;
  u32 ttl;

  Particle particle_prototype;
};


struct Particles
{
  // TODO: Make these arrays stay contiguous with deletes

  ParticleSource sources[MAX_PARTICLE_SOURCES];
  u32 next_free_particle_source;

  Particle particles[MAX_PARTICLES];
  u32 next_particle_to_use;

  Bitmap spark_bitmap;
  Bitmap cross_bitmap;
};