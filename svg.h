enum StrokeLinecap
{
  LINECAP_ROUND,
  LINECAP_SQUARE
};


struct SVGStyle
{
  u32 stroke_width;
  V4 stroke_colour;
  StrokeLinecap stroke_linecap;
  StrokeLinecap stroke_linejoin;

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


enum SVGOperationType
{
  SVG_OP_PATH
};


struct SVGOperation
{
  SVGOperationType type;

  union {
    SVGPath path;
  };

  SVGOperation *next;
};

