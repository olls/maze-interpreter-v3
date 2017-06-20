bool
init_freetype(FT_Library *result)
{
  return FT_Init_FreeType(result) != 0;
}


const char FONT_FACE[] = "/usr/share/fonts/truetype/freefont/FreeSans.ttf";


bool
load_font(FT_Library *font_library, FT_Face *result)
{
  bool error = false;

  if (FT_New_Face(*font_library, FONT_FACE, 0, result) != 0)
  {
    printf("Error: Unable to load font face \"%s\"\n", FONT_FACE);
    error = true;
  }
  else
  {
    FT_Set_Pixel_Sizes(*result, 0, 48);
  }

  return error;
}


void
draw_string(FT_Library *font_library, FT_Face face, ConstString string)
{
  glPushAttrib(GL_ALL_ATTRIB_BITS); {

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint tex;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glEnable(GL_TEXTURE_2D);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glColor3f(0, 0, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    V2 pos = {0, 0};
    V2 size = {5, 5};

    for (u32 char_index = 0;
         char_index < string.length;
         ++char_index)
    {
      char character = string.text[char_index];

      if (FT_Load_Char(face, character, FT_LOAD_RENDER)) {
        printf("Error: Unable to load character \"%c\"\n", character);
      }
      else
      {
        FT_GlyphSlot glyph = face->glyph;

        glTexImage2D(
          GL_TEXTURE_2D,
          0,
          GL_RED,
          glyph->bitmap.width,
          glyph->bitmap.rows,
          0,
          GL_RED,
          GL_UNSIGNED_BYTE,
          glyph->bitmap.buffer
        );

        r32 x2 =  pos.x + glyph->bitmap_left * size.x;
        r32 y2 = -pos.y - glyph->bitmap_top  * size.y;
        r32 w = glyph->bitmap.width * size.x;
        r32 h = glyph->bitmap.rows  * size.y;

        glBegin(GL_QUADS);
          glTexCoord2f(0, 0);
          glVertex3f(x2    , -y2    , 0);
          glTexCoord2f(1, 0);
          glVertex3f(x2 + w, -y2    , 0);
          glTexCoord2f(1, 1);
          glVertex3f(x2 + w, -y2 - h, 0);
          glTexCoord2f(0, 1);
          glVertex3f(x2    , -y2 - h, 0);
        glEnd();

        pos.x += (glyph->advance.x/64) * size.x;
        pos.y += (glyph->advance.y/64) * size.y;
      }
    }

  } glPopAttrib();
}