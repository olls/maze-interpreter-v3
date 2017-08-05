b32
not_label(u8 c)
{
  b32 result = true;

  if (is_letter(c) || is_num(c) || c == ':' || c == '-' || c == '_')
  {
    result = false;
  }

  return result;
}


const u8 *
get_label(const u8 *start, const u8 *end_f, String *result)
{
  const u8 *c = start;
  consume_until(c, is_letter, end_f);

  result->text = c;

  consume_until(c, not_label, end_f);
  result->length = c - result->text;

  return c;
}


XMLTag *
parse_tag_tokens(Memory *memory, File *file, u32 *n_tokens)
{
  // Creates a array of tokens, each representing "< ... >", sequentially in memory
  // TODO: This really doesn't handle comments well.

  const u8 *end_f = file->text + file->size;
  const u8 *c = file->text;

  XMLTag *result = 0;

  while (c < end_f)
  {
    consume_until_char(c, '<', end_f);
    if (c == end_f) break;
    ++c;

    XMLTag *tag = push_struct(memory, XMLTag);
    tag->file_start = c;

    consume_until_char(c, '>', end_f);
    if (c == end_f) break;
    tag->file_end = c;

    ++(*n_tokens);
    if (!result)
    {
      result = tag;
    }

    if (tag->file_start[0] == '?' && tag->file_end[-1] == '?')
    {
      // XML declaration
      log(L_XMLTokens, u8("XML declaration"));

      tag->type = XML_DECLARATION;

      ++tag->file_start;
      --tag->file_end;
    }
    else if (tag->file_end[-1] == '/')
    {
      // Empty-element tag
      log(L_XMLTokens, u8("Empty-element tag"));

      tag->type = XML_EMPTY_ELEMENT;

      --tag->file_end;
    }
    else if (str_eq(tag->file_start, u8("!--"), 3) && str_eq((tag->file_end - 2), u8("--"), 2))
    {
      // Comment
      log(L_XMLTokens, u8("Comment"));

      tag->type = XML_COMMENT;

      tag->file_start += 3;
      tag->file_end -= 2;
    }
    else if (tag->file_start[0] == '/')
    {
      // Tag
      log(L_XMLTokens, u8("Tag end"));

      tag->type = XML_ELEMENT_END;
    }
    else
    {
      // Tag
      log(L_XMLTokens, u8("Tag"));

      tag->type = XML_ELEMENT_START;
    }

    if (tag->type == XML_EMPTY_ELEMENT ||
        tag->type == XML_ELEMENT_START ||
        tag->type == XML_ELEMENT_END)
    {
      get_label(tag->file_start, tag->file_end, &tag->name);

      log(L_XMLTokens, u8(" %.*s"), tag->name.length, tag->name.text);
    }

    ++c;
  }

  return result;
}


void
parse_tag_attrs(Memory *memory, XMLTag *tag)
{
  const u8 *c = tag->name.text + tag->name.length;

  tag->attrs = 0;

  XMLAttr tmp_attr;

  while (c < tag->file_end)
  {
    const u8 *new_c = get_label(c, tag->file_end, &tmp_attr.name);
    if (tmp_attr.name.length == 0 || c == tag->file_end) break;
    c = new_c;

    consume_until_char(c, '=', tag->file_end);
    if (c == tag->file_end) break;
    ++c;

    consume_until_char(c, '"', tag->file_end);
    if (c == tag->file_end) break;
    ++c;

    tmp_attr.value.text = c;

    consume_until_char(c, '"', tag->file_end);
    if (c == tag->file_end) break;

    tmp_attr.value.length = c - tmp_attr.value.text;

    ++c;

    XMLAttr *attr = push_struct(memory, XMLAttr);
    *attr = tmp_attr;

    attr->next_attr = tag->attrs;
    tag->attrs = attr;
  }
}


void
parse_xml_tags(Memory *memory, XMLTag *start_of_tokens, XMLTag *end_of_tokens)
{
  for (XMLTag *tag_token = start_of_tokens;
       tag_token < end_of_tokens;
       ++tag_token)
  {
    if (tag_token->type == XML_ELEMENT_START ||
        tag_token->type == XML_EMPTY_ELEMENT)
    {
      parse_tag_attrs(memory, tag_token);
    }
  }
}


XMLTag *
parse_xml_structure(Memory *memory, XMLTag *parent, XMLTag *start_of_tokens, XMLTag *end_of_tokens, u32 level=0)
{
  XMLTag *tag_token;

  for (tag_token = start_of_tokens;
       tag_token < end_of_tokens;
       ++tag_token)
  {
    b32 loop_break = false;

    tag_token->first_child = 0;

    switch (tag_token->type)
    {
      case (XML_ELEMENT_START):
      {
        tag_token->type = XML_ELEMENT;

        tag_token->next_sibling = parent->first_child;
        parent->first_child = tag_token;

        log_ind(L_XML, level, u8("Starting: %.*s {"), tag_token->name.length, tag_token->name.text);
        tag_token = parse_xml_structure(memory, tag_token, tag_token+1, end_of_tokens, level+1);
        log_ind(L_XML, level, u8("}"));
      } break;

      case (XML_ELEMENT_END):
      {
        if (str_eq(&parent->name, &tag_token->name))
        {
          loop_break = true;
        }
        else
        {
          log_ind(L_XML, level, u8("Error in XML: Closing tag doesn't match opening tag"));
          loop_break = true;
        }
      } break;

      case (XML_EMPTY_ELEMENT):
      {
        log_ind(L_XML, level, u8("Found: %.*s"), tag_token->name.length, tag_token->name.text);

        tag_token->next_sibling = parent->first_child;
        parent->first_child = tag_token;
      } break;

      case (XML_DECLARATION):
      case (XML_COMMENT):
      {} break;

      default:
      {
        printf("Error: Shouldn't be looping over already parsed tokens (type = %d)\n", tag_token->type);
      } break;
    }

    if (loop_break)
    {
      break;
    }
  }

  return tag_token;
}


void
parse_xml(Memory *memory, File *file, XMLTag **root)
{
  u32 n_tokens = 0;
  *root = parse_tag_tokens(memory, file, &n_tokens);

  XMLTag *start_of_tokens = *root;
  XMLTag *end_of_tokens = *root + n_tokens;

  parse_xml_tags(memory, start_of_tokens, end_of_tokens);

  parse_xml_structure(memory, *root, start_of_tokens, end_of_tokens);
}


XMLTag *
load_xml(const u8 filename[], Memory *memory)
{
  File file;
  open_file(filename, &file);

  XMLTag *result;
  parse_xml(memory, &file, &result);

  // Leave file open, as we don't copy the data out of the memory map

  return result;
}


void
test_traverse_xml_struct(XMLTag *parent, u32 level=0)
{
  XMLTag *child = parent->first_child;
  while (child)
  {
    log_ind(L_XML, level, u8("%.*s"), child->name.length, child->name.text);

    XMLAttr *attr = child->attrs;
    while (attr)
    {
      log_ind(L_XML, level + 1, u8("'%.*s' = '%.*s'"), attr->name.length, attr->name.text, attr->value.length, attr->value.text);
      attr = attr->next_attr;
    }

    if (child->type == XML_ELEMENT)
    {
      test_traverse_xml_struct(child, level + 2);
    }

    child = child->next_sibling;
  }
}


String
get_attr_value(XMLTag *tag, String name)
{
  String result = {.text = 0, .length = 0};

  XMLAttr *attr = tag->attrs;
  while (attr)
  {
    if (str_eq(&attr->name, &name))
    {
      result = attr->value;
      break;
    }

    attr = attr->next_attr;
  }

  return result;
}