mat2
multiply(mat2 A, mat2 B)
{
  mat2 result;

  result.a.x = A.a.x*B.a.x + A.a.y*B.b.x;
  result.a.y = A.a.x*B.a.y + A.a.y*B.b.y;

  result.b.x = A.b.x*B.a.x + A.b.y*B.b.x;
  result.b.y = A.b.x*B.a.y + A.b.y*B.b.y;

  return result;
}


mat3
multiply(mat3 A, mat3 B)
{
  mat3 result;

  result.a.x = A.a.x*B.a.x + A.a.y*B.b.x + A.a.z*B.c.x;
  result.a.y = A.a.x*B.a.y + A.a.y*B.b.y + A.a.z*B.c.y;
  result.a.z = A.a.x*B.a.z + A.a.y*B.b.z + A.a.z*B.c.z;

  result.b.x = A.b.x*B.a.x + A.b.y*B.b.x + A.b.z*B.c.x;
  result.b.y = A.b.x*B.a.y + A.b.y*B.b.y + A.b.z*B.c.y;
  result.b.z = A.b.x*B.a.z + A.b.y*B.b.z + A.b.z*B.c.z;

  result.c.x = A.c.x*B.a.x + A.c.y*B.b.x + A.c.z*B.c.x;
  result.c.y = A.c.x*B.a.y + A.c.y*B.b.y + A.c.z*B.c.y;
  result.c.z = A.c.x*B.a.z + A.c.y*B.b.z + A.c.z*B.c.z;

  return result;
}


mat4
multiply(mat4 A, mat4 B)
{
  mat4 result;

  result.a.x = A.a.x*B.a.x + A.a.y*B.b.x + A.a.z*B.c.x + A.a.w*B.d.x;
  result.a.y = A.a.x*B.a.y + A.a.y*B.b.y + A.a.z*B.c.y + A.a.w*B.d.y;
  result.a.z = A.a.x*B.a.z + A.a.y*B.b.z + A.a.z*B.c.z + A.a.w*B.d.z;
  result.a.w = A.a.x*B.a.w + A.a.y*B.b.w + A.a.z*B.c.w + A.a.w*B.d.w;

  result.b.x = A.b.x*B.a.x + A.b.y*B.b.x + A.b.z*B.c.x + A.b.w*B.d.x;
  result.b.y = A.b.x*B.a.y + A.b.y*B.b.y + A.b.z*B.c.y + A.b.w*B.d.y;
  result.b.z = A.b.x*B.a.z + A.b.y*B.b.z + A.b.z*B.c.z + A.b.w*B.d.z;
  result.b.w = A.b.x*B.a.w + A.b.y*B.b.w + A.b.z*B.c.w + A.b.w*B.d.w;

  result.c.x = A.c.x*B.a.x + A.c.y*B.b.x + A.c.z*B.c.x + A.c.w*B.d.x;
  result.c.y = A.c.x*B.a.y + A.c.y*B.b.y + A.c.z*B.c.y + A.c.w*B.d.y;
  result.c.z = A.c.x*B.a.z + A.c.y*B.b.z + A.c.z*B.c.z + A.c.w*B.d.z;
  result.c.w = A.c.x*B.a.w + A.c.y*B.b.w + A.c.z*B.c.w + A.c.w*B.d.w;

  result.d.x = A.d.x*B.a.x + A.d.y*B.b.x + A.d.z*B.c.x + A.d.w*B.d.x;
  result.d.y = A.d.x*B.a.y + A.d.y*B.b.y + A.d.z*B.c.y + A.d.w*B.d.y;
  result.d.z = A.d.x*B.a.z + A.d.y*B.b.z + A.d.z*B.c.z + A.d.w*B.d.z;
  result.d.w = A.d.x*B.a.w + A.d.y*B.b.w + A.d.z*B.c.w + A.d.w*B.d.w;

  return result;
}


void
multiply(mat2 *A, mat2 B)
{
  mat2 result = multiply(*A, B);
  *A = result;
}


void
multiply(mat3 *A, mat3 B)
{
  mat3 result = multiply(*A, B);
  *A = result;
}


void
multiply(mat4 *A, mat4 B)
{
  mat4 result = multiply(*A, B);
  *A = result;
}


vec2
transform(mat2 M, vec2 V)
{
  vec2 result;

  result.x = M.a.x*V.x + M.a.y*V.y;
  result.y = M.b.x*V.x + M.b.y*V.y;

  return result;
}


vec3
transform(mat3 M, vec3 V)
{
  vec3 result;

  result.x = M.a.x*V.x + M.a.y*V.y + M.a.z*V.z;
  result.y = M.b.x*V.x + M.b.y*V.y + M.b.z*V.z;
  result.z = M.c.x*V.x + M.c.y*V.y + M.c.z*V.z;

  return result;
}


vec4
transform(mat4 M, vec4 V)
{
  vec4 result;

  result.x = M.a.x*V.x + M.a.y*V.y + M.a.z*V.z + M.a.w*V.w;
  result.y = M.b.x*V.x + M.b.y*V.y + M.b.z*V.z + M.b.w*V.w;
  result.z = M.c.x*V.x + M.c.y*V.y + M.c.z*V.z + M.c.w*V.w;
  result.w = M.d.x*V.x + M.d.y*V.y + M.d.z*V.z + M.d.w*V.w;

  return result;
}


void
transform(mat2 M, vec2 *V)
{
  vec2 result;
  result = transform(M, *V);
  *V = result;
}


void
transform(mat3 M, vec3 *V)
{
  vec3 result;
  result = transform(M, *V);
  *V = result;
}


void
transform(mat4 M, vec4 *V)
{
  vec4 result;
  result = transform(M, *V);
  *V = result;
}


void
print_vec(vec2 v)
{
  printf("[ %7.3f, %7.3f ]\n", v.x, v.y);
}


void
print_vec(vec3 v)
{
  printf("[ %7.3f, %7.3f, %7.3f ]\n", v.x, v.y, v.z);
}


void
print_vec(vec4 v)
{
  printf("[ %7.3f, %7.3f, %7.3f, %7.3f ]\n", v.x, v.y, v.z, v.w);
}


void
print_mat(mat2 m)
{
  print_vec(m.a);
  print_vec(m.b);
}


void
print_mat(mat3 m)
{
  print_vec(m.a);
  print_vec(m.b);
  print_vec(m.c);
}


void
print_mat(mat4 m)
{
  print_vec(m.a);
  print_vec(m.b);
  print_vec(m.c);
  print_vec(m.d);
}
