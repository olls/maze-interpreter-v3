b32
tag_cmp(const u8 *a, const u8 *b)
{
  return str_eq(a, b, 4);
}


OTF_TableRecord *
get_table_record(OTF_OffsetTable *otf, const u8 tag[])
{
  u32 num_tables = to_little_endian(otf->num_tables);
  OTF_TableRecord *result = 0;

  OTF_TableRecord *table_record;
  for (u32 table_n = 0;
       table_n < num_tables;
       ++table_n)
  {
    table_record = otf->table_records + table_n;
    if (tag_cmp(table_record->tag, tag))
    {
      result = table_record;
      break;
    }
  }

  return result;
}


b32
get_table(Font *font, const u8 tag[], void **table)
{
  b32 success = true;

  OTF_TableRecord *table_record = get_table_record(font->otf, tag);
  if (table_record == 0)
  {
    log(L_OpenType, u8("Table record %.4s missing"), tag);
    *table = 0;
    success &= false;
  }
  else
  {
    *table = (void *)(font->file->text + to_little_endian(table_record->offset));
  }

  return success;
}


b32
locate_required_tables(Font *font)
{
  b32 success = true;

  OTF_Tables *tables = &font->table_ptrs;

  success &= get_table(font, u8("cmap"), (void **)&tables->cmap);
  success &= get_table(font, u8("head"), (void **)&tables->head);
  success &= get_table(font, u8("hhea"), (void **)&tables->hhea);
  success &= get_table(font, u8("hmtx"), (void **)&tables->hmtx);
  success &= get_table(font, u8("maxp"), (void **)&tables->maxp);
  success &= get_table(font, u8("name"), (void **)&tables->name);
  success &= get_table(font, u8("OS/2"), (void **)&tables->os_2);
  success &= get_table(font, u8("post"), (void **)&tables->post);

  return success;
}


b32
locate_required_true_type_tables(Font *font)
{
  b32 success = true;

  OTF_TrueType_Tables *tables = &font->true_type_table_ptrs;

  success &= get_table(font, u8("cvt "), (void **)&tables->cvt_);
  success &= get_table(font, u8("fpgm"), (void **)&tables->fpgm);
  success &= get_table(font, u8("glyf"), (void **)&tables->glyf);
  success &= get_table(font, u8("loca"), (void **)&tables->loca);
  success &= get_table(font, u8("prep"), (void **)&tables->prep);
  success &= get_table(font, u8("gasp"), (void **)&tables->gasp);

  return success;
}


b32
locate_sfnt_version_tables(Font *font)
{
  b32 success = true;

  if (tag_cmp(font->otf->sfnt_version, (const u8 *)"\0\1\0\0") ||
      tag_cmp(font->otf->sfnt_version, (const u8 *)"true"))
  {
    log(L_OpenType, u8("Parsing TrueType font outline data."));

    success &= locate_required_true_type_tables(font);
  }
  else if (tag_cmp(font->otf->sfnt_version, (const u8 *)"OTTO"))
  {
    log(L_OpenType, u8("CFF font outline data is not supported."));
    success = false;
  }
  else
  {
    log(L_OpenType, u8("Error in font file offset table, SFNT version is not supported: %.4s, 0x%.2x%.2x%.2x%.2x"),
        font->otf->sfnt_version, font->otf->sfnt_version[0],
                                 font->otf->sfnt_version[1],
                                 font->otf->sfnt_version[2],
                                 font->otf->sfnt_version[3]);
    success = false;
  }

  return success;
}


b32
locate_supported_cmap_encoding(Font *font)
{
  b32 success = true;
  OTF_CMAP_Table *cmap_table = font->table_ptrs.cmap;

  if (to_little_endian(cmap_table->version) != 0)
  {
    log(L_OpenType, u8("Miss-formatted font, cmap_table version is not equal to 0"));
    success = false;
  }
  else
  {
    log(L_OpenType, u8("Reading OTF encoding types:"));

    OTF_CMAP_SubTable_Format12 *format_12 = 0;

    for (u32 encoding_num = 0;
         encoding_num < to_little_endian(cmap_table->num_tables);
         ++encoding_num)
    {
      OTF_EncodingRecord *encoding_record = cmap_table->encoding_records + encoding_num;

      u16 platform_id = to_little_endian(encoding_record->platform_id);
      u16 encoding_id = to_little_endian(encoding_record->encoding_id);
      log(L_OpenType, u8("platform ID: %u, encoding ID: %u"), platform_id, encoding_id);

      u8 *sub_table = (u8 *)cmap_table + to_little_endian(encoding_record->offset);

      u16 version = to_little_endian(*(u16 *)sub_table);
      log(L_OpenType, u8("  version: %u"), version);

      if ((platform_id == 0 || platform_id == 3) &&
          encoding_id == 10 &&
          version == 12)
      {
        format_12 = (OTF_CMAP_SubTable_Format12 *)sub_table;
        break;
      }
    }

    if (format_12 == 0)
    {
      log(L_OpenType, u8("Font does not contain any currently supported CMAP sub-table versions."));
      success = false;
    }
    else
    {
      log(L_OpenType, u8("Found supported CMAP sub-table format 12"));
      font->cmap_subtable_12 = format_12;
    }
  }

  return success;
}


b32
parse_otf(File *file, OTF_OffsetTable *otf, Font *result)
{
  b32 success = true;

  result->file = file;
  result->otf = otf;

  success &= locate_required_tables(result);
  success &= locate_sfnt_version_tables(result);

  if (!success)
  {
    log(L_OpenType, u8("Missing required tables in OTF file."));
  }
  else
  {
    success &= locate_supported_cmap_encoding(result);
  }

  return success;
}


b32
parse_otf_file(const u8 filename[], Font *result)
{
  b32 success = true;
  log(L_OpenType, u8("Parsing %s"), filename);

  File file;
  open_file(filename, &file);

  // Test if the file is a font collection file
  TTC_Header *ttc = (TTC_Header *)file.text;

  if (tag_cmp(ttc->ttc_tag, (const u8 *)"ttcf"))
  {
    log(L_OpenType, u8("Parsing TrueType Collection; not currently supported."));
    success &= false;
  }
  else
  {
    OTF_OffsetTable *otf = (OTF_OffsetTable *)file.text;
    success &= parse_otf(&file, otf, result);
  }

  return success;
}


OTF_GLYF_Simple_Ptrs
read_simple_true_type_glyph(OTF_GLYF_Header *glyph_header, GlyphLocation glyph_location)
{
  // Read the packed simple glyph layout into a struct of pointers
  OTF_GLYF_Simple_Ptrs result;

  // Go to end of glyph header
  u8 *pos = (u8 *)glyph_header + sizeof(OTF_GLYF_Header);

  result.number_of_contours = to_little_endian(glyph_header->number_of_contours);
  result.end_points_of_contours = (u16 *)pos;
  pos += result.number_of_contours * sizeof(u16);

  result.instruction_length = to_little_endian(*(u16 *)pos);
  pos += sizeof(u16);

  if (result.instruction_length == 0)
  {
    result.instructions = 0;
  }
  else
  {
    result.instructions = (u8 *)pos;
    pos += result.instruction_length * sizeof(u8);
  }

  result.n_points = to_little_endian(result.end_points_of_contours[result.number_of_contours-1]) + 1;

  // There is one flag per point, but n_flags may be less than n_points if the REPEAT flag is set on any flags.
  u32 n_flags = result.n_points;
  result.coordinate_flags = (OTF_GLYF_SimpleFlags *)pos;

  // Loop through the flags looking at the REPEAT and OTF_GLYF_FLAG_X_*
  //   flags to find the size of the flags and x_coordinates arrays.
  // This is so we can find the start point of all three arrays, and loop
  //   through them in parallel later on.

  u32 logical_flag_number = 0;
  u32 physical_flag_position = 0;

  u32 x_coordinates_size = 0;

  while (logical_flag_number < n_flags)
  {
    OTF_GLYF_SimpleFlags *current_flag = result.coordinate_flags + physical_flag_position;
    ++logical_flag_number;
    ++physical_flag_position;

    u8 repeat_count = 0;

    if (*current_flag & OTF_GLYF_FLAG_REPEAT_FLAG)
    {
      // Next byte is the repeat count
      physical_flag_position += 1;
      repeat_count = *(u8 *)(current_flag + 1);
      logical_flag_number += repeat_count;
    }

    u32 this_coordinate_size = 0;

    if ( !(*current_flag & OTF_GLYF_FLAG_X_SHORT_VECTOR) )
    {
      if (*current_flag & OTF_GLYF_FLAG_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR)
      {
        // If X_SHORT_VECTOR is not set and this bit is set, then the current
        //   x-coordinate is the same as the previous x-coordinate.
        this_coordinate_size = 0;
      }
      else
      {
        // If X_SHORT_VECTOR is not set and this bit is also not set, the
        //   current x-coordinate is a signed 16-bit delta vector.
        this_coordinate_size = sizeof(s16);
      }
    }
    else
    {
      // If X_SHORT_VECTOR is set, this bit describes the sign of
      //   the value, with 1 equalling positive and 0 negative.
      this_coordinate_size = sizeof(u8);
    }

    x_coordinates_size += this_coordinate_size * (1 + repeat_count);
  }

  u32 flags_size = physical_flag_position;
  pos += flags_size * sizeof(OTF_GLYF_SimpleFlags);

  result.x_coordinates = (u8 *)pos;
  pos += x_coordinates_size;
  result.y_coordinates = (u8 *)pos;

  return result;
}


r32
parse_coordinate_flags(OTF_GLYF_SimpleFlags flag,
                       u8 *coordinates_list,
                       u32 *coordinate_index,
                       u8 flag_short_vector,
                       u8 flag_is_same_or_positive_short_vector)
{
  r32 delta = 0;

  u8 *coordinate_pos = coordinates_list + *coordinate_index;

  if ( !(flag & flag_short_vector) )
  {
    if (flag & flag_is_same_or_positive_short_vector)
    {
      // If X_SHORT_VECTOR is not set and this bit is set, then the current
      //   x-coordinate is the same as the previous x-coordinate.
      delta = 0;

      log(L_OpenType, u8("  Repeating point"));
    }
    else
    {
      // If X_SHORT_VECTOR is not set and this bit is also not set, the
      //   current x-coordinate is a signed 16-bit delta vector.
      delta = to_little_endian(*(s16 *)coordinate_pos);

      *coordinate_index += sizeof(s16);
      log(L_OpenType, u8("  s16 delta value"));
    }
  }
  else
  {
    // If X_SHORT_VECTOR is set, this bit describes the sign of
    //   the value, with 1 equalling positive and 0 negative.
    b32 sign = flag & flag_is_same_or_positive_short_vector;
    delta = *(u8 *)coordinate_pos * (sign ? +1 : -1);

    *coordinate_index += sizeof(u8);
    log(L_OpenType, u8("  u8 delta value"));
  }

  return delta;
}


struct GlyphPointReader
{
  u32 next_point_number;

  u32 current_x_index;
  u32 current_y_index;

  vec2 point_delta;

  u32 next_flag_index;

  u32 flag_repeats_left;
  OTF_GLYF_SimpleFlags repeating_flag;
};

b32
read_next_glyph_point(OTF_GLYF_Simple_Ptrs *glyph_pointers, GlyphPointReader *glyph_point_reader)
{
  b32 success = true;

  if (glyph_point_reader->next_point_number == glyph_pointers->n_points)
  {
    // Reached the end of the points
    success = false;
  }
  else
  {
    ++glyph_point_reader->next_point_number;

    OTF_GLYF_SimpleFlags current_flag;
    if (glyph_point_reader->flag_repeats_left > 0)
    {
      // Currently repeating a flag.
      current_flag = glyph_point_reader->repeating_flag;
      --glyph_point_reader->flag_repeats_left;
    }
    else
    {
      // Not currently repeating a flag, get the next flag.
      u32 current_flag_index = glyph_point_reader->next_flag_index;
      current_flag = glyph_pointers->coordinate_flags[current_flag_index];
      glyph_point_reader->next_flag_index += 1;

      if (current_flag & OTF_GLYF_FLAG_REPEAT_FLAG)
      {
        // Set the repeats left to start repeating in the next iteration
        glyph_point_reader->flag_repeats_left = *((u8 *)glyph_pointers->coordinate_flags + current_flag_index + 1);
        glyph_point_reader->next_flag_index += 1;
      }
    }

    glyph_point_reader->point_delta.x = parse_coordinate_flags(current_flag, glyph_pointers->x_coordinates, &glyph_point_reader->current_x_index,
                                                               OTF_GLYF_FLAG_X_SHORT_VECTOR, OTF_GLYF_FLAG_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR);

    glyph_point_reader->point_delta.y = parse_coordinate_flags(current_flag, glyph_pointers->y_coordinates, &glyph_point_reader->current_y_index,
                                                               OTF_GLYF_FLAG_Y_SHORT_VECTOR, OTF_GLYF_FLAG_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR);
  }

  return success;
}


GlyphLocation
get_glyph_location(Font *font, u32 glyph_index)
{
  // Get the end position of the glyph by looking at the start position of the next glyph.
  GlyphLocation result;

  OTF_HEAD_Table *head_table = font->table_ptrs.head;
  OTF_LOCA_Table *loca_table = font->true_type_table_ptrs.loca;

  if (to_little_endian(head_table->index_to_loc_format) == 1)
  {
    result.start_position = to_little_endian(loca_table->offsets_long[glyph_index]);
    result.end_position = to_little_endian(loca_table->offsets_long[glyph_index + 1]);
  }
  else
  {
    result.start_position = to_little_endian(loca_table->offsets_short[glyph_index]);
    result.end_position = to_little_endian(loca_table->offsets_short[glyph_index + 1]);
  }

  return result;
}


b32
get_true_type_glyph(Font *font, u32 glyph_index)
{
  b32 success = true;

  GlyphLocation glyph_location = get_glyph_location(font, glyph_index);

  OTF_GLYF_Table *glyf_table = font->true_type_table_ptrs.glyf;
  OTF_GLYF_Header *glyph_header = (OTF_GLYF_Header *)((u8 *)glyf_table->glyphs + glyph_location.start_position);

  log(L_OpenType, u8("x_min: %d"), to_little_endian(glyph_header->x_min));
  log(L_OpenType, u8("y_min: %d"), to_little_endian(glyph_header->y_min));
  log(L_OpenType, u8("x_max: %d"), to_little_endian(glyph_header->x_max));
  log(L_OpenType, u8("y_max: %d"), to_little_endian(glyph_header->y_max));

  if (to_little_endian(glyph_header->number_of_contours) >= 0)
  {
    log(L_OpenType, u8("Getting simple glyph"));

    OTF_GLYF_Simple_Ptrs simple_glyph = read_simple_true_type_glyph(glyph_header, glyph_location);
    log(L_OpenType, u8("number_of_contours: %d, number_of_points: %u"), simple_glyph.number_of_contours, simple_glyph.n_points);

    // Read glyph contour coordinates into a "glyph cache" which we can then
    //   render from (the glyph cache could possibly be a VBO)

    GlyphPointReader glyph_point_reader = {};
    b32 got_point = true;
    vec2 point = {0, 0};
    while (got_point)
    {
      got_point = read_next_glyph_point(&simple_glyph, &glyph_point_reader);
      if (got_point)
      {
        point += glyph_point_reader.point_delta;

        log(L_OpenType, u8("Point delta: (%10.3f, %10.3f), abs: (%10.3f, %10.3f), on curve: %d"), glyph_point_reader.point_delta.x, glyph_point_reader.point_delta.y, control_point->point.x, control_point->point.y, control_point->on_curve);
      }
    }
  }
  else
  {
    log(L_OpenType, u8("Getting Composite glyph"));

  }

  return success;
}


u32
get_glyph_index(Font *font, u32 character)
{
  u32 glyph_index = 0;

  if (font->cmap_subtable_12 != 0)
  {
    OTF_CMAP_SubTable_Format12 *mappings = font->cmap_subtable_12;

    // Find group containing character
    for (u32 group_index = 0;
         group_index < to_little_endian(mappings->num_groups);
         ++group_index)
    {
      OTF_CMAP_SubTable_Format12::SequentialMapGroup *group = mappings->groups + group_index;

      u32 start_char_code = to_little_endian(group->start_char_code);
      u32 end_char_code = to_little_endian(group->end_char_code);
      u32 start_glyph_id = to_little_endian(group->start_glyph_id);

      if (start_char_code <= character && character <= end_char_code)
      {
        u32 character_position_in_group = character - start_char_code;
        glyph_index = start_glyph_id + character_position_in_group;
        break;
      }
    }
  }

  OTF_MAXP_Table *maxp_table = font->table_ptrs.maxp;
  u16 num_glyphs = maxp_table->num_glyphs;

  if (glyph_index >= num_glyphs)
  {
    log(L_OpenType, u8("Error in get_glyph_index for character: '%u'; got index %u >= num_glyphs"),
        character, glyph_index);
    glyph_index = 0;
  }

  return glyph_index;
}


b32
get_glyph(Font *font, u32 character)
{
  b32 success = true;

  u32 glyph_index = get_glyph_index(font, character);
  log(L_OpenType, u8("Got glyph index of %u for character %u (%c)"), glyph_index, character, character);

  switch (font->sfnt_version)
  {
    case OTF_SFNT_TrueType_Outlines:
    {
      log(L_OpenType, u8("Getting glyph from TrueType font."));

      success &= get_true_type_glyph(font, glyph_index);

    } break;
    case OTF_SFNT_CFF_Data:
    {
      success = false;
      log(L_OpenType, u8("Cannot get glyph from currently unimplemented CFF font."));
    } break;
    case OTF_SFNT_Undefined:
    {
      success = false;
      log(L_OpenType, u8("Unrecognised font SFNT version while trying to get glyph."));
    } break;
  }

  return success;
}
