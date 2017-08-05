void
print_u32(u32_String string)
{
  for (u32 character_index = 0;
       character_index < string.length;
       ++character_index)
  {
    u32 character = string.text[character_index];

    // printf(character);
  }
}


// s32
// print_u32(const char32_t *format, ...)
// {
//   va_list args;
//   va_start(args, format);

//   while (*format != '\0')
//   {
//     if (*format == 'd')
//     {
//       u32 i = va_arg(args, u32);
//     }
//     else if (*format == 'c')
//     {
//       // note automatic conversion to integral type
//       u32 c = va_arg(args, u32);
//     }
//     else if (*format == 'f')
//     {
//       double d = va_arg(args, double);
//     }
//     ++format;
//   }

//   va_end(args);
// }
