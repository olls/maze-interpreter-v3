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

  char *file_start;
  char *file_end;

  XMLAttr *attrs;

  XMLTag *next_sibling;

  XMLTag *first_child;
};