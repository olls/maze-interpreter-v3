struct XMLAttr
{
  String name;
  String value;

  XMLAttr *next_attr;
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

  const u8 *file_start;
  const u8 *file_end;

  XMLAttr *attrs;

  XMLTag *next_sibling;

  XMLTag *first_child;
};