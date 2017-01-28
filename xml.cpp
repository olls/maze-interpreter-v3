b32
not_label(char c)
{
  b32 result = true;

  if (is_letter(c) || c == ':')
  {
    result = false;
  }

  return result;
}


void
get_label(char *c, char *end_f, String *result)
{
  consume_until(c, is_letter, end_f);

  result->text = c;

  consume_until(c, not_label, end_f);
  result->length = c - result->text;
}


char *
get_tag_attrs(Memory *memory, char *text, char *end_f, XMLTag *tag)
{
  tag->attrs = 0;
  tag->n_attrs = 0;

  XMLAttr tmp_attr;

  while (text < end_f)
  {
    consume_whitespace(text, end_f);

    tmp_attr.name.text = text;
    consume_while(text, is_letter, end_f);
    tmp_attr.name.length = text - tmp_attr.name.text;

    if (tmp_attr.name.length == 0 || text == end_f) break;

    consume_until_char(text, '=', end_f);
    if (text == end_f) break;

    consume_until_char(text, '"', end_f);
    if (text == end_f) break;

    tmp_attr.value = text;

    consume_until_char(text, '"', end_f);
    if (text == end_f) break;

    tmp_attr.value_len = text - tmp_attr.value;

    XMLAttr *attr = push_struct(memory, XMLAttr);
    if (tag->attrs == 0)
    {
      tag->attrs = attr;
    }

    *attr = tmp_attr;
    ++tag->n_attrs;
  }

  return text;
}


/*
    new_c = get_tag_name(c, end_f, &tag);
    if (new_c != end_f)
    {
      c = new_c;
    }

    printf("%d\n", tag.name.length);
    print(&tag.name);
    printf("\n");

    new_c = get_tag_attrs(memory, c, end_f, &tag);
    if (new_c != end_f)
    {
      c = new_c;
    }
*/


XMLTag *
parse_tag_tokens(Memory *memory, char *c, char *end_f, u32 *n_tokens)
{
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
      log(L_SVGTokens, "XML declaration");

      tag->type = XML_DECLARATION;

      ++tag->file_start;
      --tag->file_end;
    }
    else if (tag->file_end[-1] == '/')
    {
      // Empty-element tag
      log(L_SVGTokens, "Empty-element tag");

      tag->type = XML_EMPTY_ELEMENT;

      --tag->file_end;
    }
    else if (str_eq(tag->file_start, "!--", 3) && str_eq((tag->file_end - 2), "--", 2))
    {
      // Comment
      log(L_SVGTokens, "Comment");

      tag->type = XML_COMMENT;

      tag->file_start += 3;
      tag->file_end -= 2;
    }
    else if (tag->file_start[0] == '/')
    {
      // Tag
      log(L_SVGTokens, "Tag end");

      tag->type = XML_ELEMENT_END;
    }
    else
    {
      // Tag
      log(L_SVGTokens, "Tag");

      tag->type = XML_ELEMENT_START;
    }

    if (!(tag->type == XML_COMMENT ||
          tag->type == XML_DECLARATION))
    {
      get_label(tag->file_start, end_f, &tag->name);
      log(L_SVGTokens, " %.*s", tag->name.length, tag->name.text);
    }

    ++c;
  }

  return result;
}


XMLTag *
traverse_xml(XMLTag *parent, XMLTag *start_of_tokens, XMLTag *end_of_tokens, u32 level=0)
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

        log_ind(L_SVG, level, "Starting: %.*s {", tag_token->name.length, tag_token->name.text);
        tag_token = traverse_xml(tag_token, tag_token+1, end_of_tokens, level+1);
        log_ind(L_SVG, level, "}");
      } break;

      case (XML_ELEMENT_END):
      {
        if (str_eq(&parent->name, &tag_token->name))
        {
          loop_break = true;
        }
        else
        {
          log_ind(L_SVG, level, "Error in XML: Closing tag doesn't match opening tag");
          loop_break = true;
        }
      } break;

      case (XML_EMPTY_ELEMENT):
      {
        log_ind(L_SVG, level, "Found: %.*s", tag_token->name.length, tag_token->name.text);

        tag_token->next_sibling = parent->first_child;
        parent->first_child = tag_token;
      } break;

      case (XML_DECLARATION):
      case (XML_COMMENT):
      {} break;

      default:
      {
        printf("Error: Shouldn't be looping over alredy parsed tokens\n");
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
  char *end_f = file->text + file->size;

  u32 n_tokens;
  *root = parse_tag_tokens(memory, file->text, end_f, &n_tokens);

  traverse_xml(*root, *root, *root + n_tokens);
}


XMLTag *
load_xml(const char filename[], Memory *memory)
{
  File file;
  open_file(filename, &file);

  XMLTag *result;
  parse_xml(memory, &file, &result);

  return result;
}


void
test_traverse_xml_struct(LoggingChannel channel, XMLTag *parent, u32 level=0)
{
  XMLTag *child = parent->first_child;
  while (child)
  {
    log_ind(channel, level, "%.*s", child->name.length, child->name.text);

    if (child->type == XML_ELEMENT)
    {
      test_traverse_xml_struct(channel, child, level + 1);
    }

    child = child->next_sibling;
  }
}