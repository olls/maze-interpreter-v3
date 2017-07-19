void
add_point(Memory *memory, VertexBuffer *vertices, vec2 point)
{
  vec2 *new_point = push_struct(memory, vec2);
  *new_point = point;
  ++vertices->n_vertices;
}


vec2
mid_point(vec2 a, vec2 b)
{
  return 0.5*a + 0.5*b;
}


void
subdivide_bezier(vec2 p0, vec2 p1, vec2 p2, Memory *memory, VertexBuffer *vertices, u32 recursions=0)
{
  r32 flatness = abs(p0.x*(p1.y - p2.y) +
                     p1.x*(p2.y - p0.y) +
                     p2.x*(p0.y - p1.y));

  if (flatness <= LINE_SEGMENT_FLATNESS_ELIPSON ||
      recursions >= MAX_BEZIER_SUBDIVISIONS)
  {
    add_point(memory, vertices, p1);
  }
  else
  {
    vec2 p01 = mid_point(p0, p1);
    vec2 p12 = mid_point(p1, p2);
    vec2 p012 = mid_point(p01, p12);

    subdivide_bezier(p0, p01, p012, memory, vertices, recursions+1);
    subdivide_bezier(p012, p12, p2, memory, vertices, recursions+1);
  }
}


b32
bezier_to_vertices(BezierControlPoint control_points[], u32 n_control_points, Memory *memory, VertexBuffer *result)
{
  b32 success = true;

  if (n_control_points < 1)
  {
    success = false;
    result->n_vertices = 0;
  }
  else
  {
    // First control point must be an on-curve point.
    result->vertices = push_struct(memory, vec2);
    result->vertices[0] = control_points[0].point;
    result->n_vertices = 1;

    for (u32 control_point_n = 1;
         control_point_n < n_control_points;
         ++control_point_n)
    {
      BezierControlPoint point = control_points[control_point_n];

      if (point.on_curve)
      {
        add_point(memory, result, point.point);
      }
      else
      {
        // Get the points on either side of this (p1) control point.
        // If the point is an on-curve point, then use it as p0 (or p2)
        // If the point is an off-curve point, then get p0 (or p2) as the mid-point between p1 and p0 (or p2).
        // We can assume there is a point on either side, as the control_points array forms a loop.

        u32 p0_index;
        if (control_point_n > 0)
        {
          p0_index = control_point_n - 1;
        }
        else
        {
          p0_index = n_control_points - 1;
        }

        u32 p2_index;
        if (control_point_n + 1 < n_control_points)
        {
          p2_index = control_point_n + 1;
        }
        else
        {
          p2_index = 0;
        }

        BezierControlPoint p0 = control_points[p0_index];
        BezierControlPoint p1 = point;
        BezierControlPoint p2 = control_points[p2_index];

        if (!p0.on_curve)
        {
          p0.point = mid_point(p0.point, p1.point);
        }

        if (!p2.on_curve)
        {
          p2.point = mid_point(p1.point, p2.point);
        }

        subdivide_bezier(p0.point, p1.point, p2.point, memory, result);
      }
    }
  }

  return success;
}