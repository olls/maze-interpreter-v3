enum FunctionType
{
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
  FUNCTION_GREATER,
  FUNCTION_SIGNAL
};


const uint32_t MAX_FUNCTIONS = (26*2) * ((26*2) + 10); // [A-z][A-z0-9]
struct Function
{
  char name[2];
  FunctionType type;
  union
  {
    uint32_t value;
    struct
    {
      uint32_t condition_value;
      Direction true_direction;
      Direction false_direction;
    } conditional;
  };
};
