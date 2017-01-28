struct XMLAttr
{
  String name;

  char *value;
  u32 value_len;
};


enum XMLTagType
{
  XML_DECLARATION,
  XML_EMPTY_ELEMENT,
  XML_COMMENT,
  XML_ELEMENT,
  XML_ELEMENT_START,
  XML_ELEMENT_END
};


struct XMLTag
{
  String name;

  XMLTagType type;

  char *file_start;
  char *file_end;

  XMLAttr *attrs;
  u32 n_attrs;

  XMLTag *next_sibling;

  XMLTag *first_child;
};