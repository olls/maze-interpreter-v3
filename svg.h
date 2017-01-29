struct LineSegment
{
  V2 start;
  V2 end;
};


struct SVGPath
{
  LineSegment *segments;
  u32 n_segments;
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

