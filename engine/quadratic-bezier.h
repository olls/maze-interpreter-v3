const u32 MAX_BEZIER_SUBDIVISIONS = 512;
const r32 LINE_SEGMENT_FLATNESS_ELIPSON = .001;

struct BezierControlPoint
{
  vec2 point;
  b32 on_curve;
};