#define STACK_STR(name, lit) String (name); { char s[] = (lit); (name).text = s; } (name).length = strlen(lit);


s32
print(String *string)
{
    return printf("%.*s", string->length, string->text);
}


b32
str_eq(String *a, String *b)
{
  b32 result;
  u32 length;
  if (a->length != b->length)
  {
    result = false;
  }
  else
  {
    result = str_eq(a->text, b->text, a->length);
  }

  return result;
}