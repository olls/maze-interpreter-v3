struct mat2
{
  union
  {
    vec2 rs[2];
    float es[4];
    struct
    {
      vec2 a;
      vec2 b;
    };
  };
};


struct mat3
{
  union
  {
    vec3 rs[3];
    float es[9];
    struct
    {
      vec3 a;
      vec3 b;
      vec3 c;
    };
  };
};


struct mat4
{
  union
  {
    vec4 rs[4];
    float es[16];
    struct
    {
      vec4 a;
      vec4 b;
      vec4 c;
      vec4 d;
    };
  };
};