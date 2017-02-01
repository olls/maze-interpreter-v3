enum SVGStrokeLinecap
{
  LINECAP_BUTT,
  LINECAP_ROUND,
  LINECAP_SQUARE
};


enum SVGStrokeLinejoin
{
  LINEJOIN_MITER,
  LINEJOIN_ROUND,
  LINEJOIN_SQUARE
};


struct SVGStyle
{
  u32 stroke_width;
  V4 stroke_colour;
  SVGStrokeLinecap stroke_linecap;
  SVGStrokeLinejoin stroke_linejoin;

  b32 filled;
  V4 fill_colour;
};


struct LineSegment
{
  V2 start;
  V2 end;
};


struct SVGPath
{
  LineSegment *segments;
  u32 n_segments;

  SVGStyle style;
};


struct SVGRect
{
  Rectangle rect;

  SVGStyle style;
};


struct SVGCircle
{
  V2 position;
  r32 radius;

  SVGStyle style;
};


enum SVGOperationType
{
  SVG_OP_PATH,
  SVG_OP_RECT,
  SVG_OP_CIRCLE
};


struct SVGOperation
{
  // TODO: Move SVGStyle into here instead of duplicating it in all the shape structs

  SVGOperationType type;

  union {
    SVGPath path;
    SVGRect rect;
    SVGCircle circle;
  };

  SVGOperation *next;
};

