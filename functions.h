enum FunctionType
{
  FUNCTION_NULL,
  FUNCTION_ASSIGNMENT,
  FUNCTION_INCREMENT,
  FUNCTION_DECREMENT,
  FUNCTION_MULTIPLY,
  FUNCTION_DIVIDE,
  FUNCTION_LESS,
  FUNCTION_LESS_EQUAL,
  FUNCTION_EQUAL,
  FUNCTION_NOT_EQUAL,
  FUNCTION_GREATER_EQUAL,
  FUNCTION_GREATER
};


const u32 MAX_FUNCTIONS = (26*2) * ((26*2) + 10); // [A-z][A-z0-9]
struct Function
{
  char name[2];
  FunctionType type;
  union
  {
    s32 value;
    struct
    {
      s32 value;
      V2 true_direction;
      b32 else_exists;
      V2 false_direction;
    } conditional;
  };
};


struct Functions
{
  Function hash_map[MAX_FUNCTIONS];
  u32 n_functions;
};