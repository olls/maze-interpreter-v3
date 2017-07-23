struct XMLAttr
{
  ConstString name;
  ConstString value;

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
  ConstString name;

  XMLTagType type;

  const char *file_start;
  const char *file_end;

  XMLAttr *attrs;

  XMLTag *next_sibling;

  XMLTag *first_child;
};