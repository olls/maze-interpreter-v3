u32
get_function_index(char name[2])
{
  u32 result;

  if (name[0] <= 'Z')
  {
    result = name[0] - 'A';
  }
  else
  {
    result = name[0] - 'a';
  }
  result *= ((26 * 2) + 10);  // The number of combinations in the second digit.

  if (name[1] <= '9')
  {
    result += name[1] - '0';
  }
  else if (name[1] <=  'Z')
  {
    result += name[1] - 'A';
  }
  else
  {
    result += name[1] - 'a';
  }

  assert(result < MAX_FUNCTIONS);
  return result;
}
