#define String(lit) (ConstString){ .text = lit, .length = (u32)strlen(lit) }


s32
print(ConstString *string)
{
    return printf("%.*s", string->length, string->text);
}

s32
print(String *string)
{
  return print((ConstString *)string);
}


b32
str_eq(ConstString *a, ConstString *b)
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
str_eq(ConstString a, ConstString b)
{
  return str_eq(&a, &b);
}

b32
str_eq(String *a, ConstString b)
{
  return str_eq((ConstString *)a, &b);
}

b32
str_eq(String a, ConstString b)
{
  return str_eq((ConstString *)&a, &b);
}

b32
str_eq(String *a, String *b)
{
  return str_eq((ConstString *)a, (ConstString *)b);
}

b32
str_eq(char *a, ConstString b)
{
  return str_eq((String){ .text = a, .length = b.length }, b);
}

b32
str_eq(const char *a, ConstString b)
{
  return str_eq((ConstString){ .text = a, .length = b.length }, b);
}