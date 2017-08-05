#define String(literal) create_string(literal, (u32)strlen(literal))

String
create_string(const u8 *literal, u32 length)
{
  return (String){literal, length};
}

String
create_string(const char *literal, u32 length)
{
  return (String){(const u8 *)literal, length};
}


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

b32
str_eq(String a, String b)
{
  return str_eq(&a, &b);
}

b32
str_eq(u8 *a, String b)
{
  return str_eq((String){ .text = a, .length = b.length }, b);
}

b32
str_eq(const u8 *a, String b)
{
  return str_eq((String){ .text = a, .length = b.length }, b);
}