// https://www.microsoft.com/typography/otspec/otff.htm


typedef s16 OTF_shortFrac;     // 16-bit signed fraction
typedef s32 OTF_Fixed;         // 16.16-bit signed fixed-point number
typedef s16 OTF_FWord;         // 16-bit signed integer that describes a quantity in FUnits, the smallest measurable distance in em space.
typedef u16 OTF_uFWord;        // 16-bit unsigned integer that describes a quantity in FUnits, the smallest measurable distance in em space.
typedef s16 OTF_F2Dot14;       // 16-bit signed fixed number with the low 14 bits representing fraction.
typedef s64 OTF_LongDateTime;  // The long internal format of a date in seconds since 12:00 midnight, January 1, 1904. It is represented as
                               //   a signed 64-bit integer.


// Required tables:

// 'cmap' Character to glyph mapping
struct OTF_EncodingRecord
{
  u16 platform_id;  // Platform ID.
  u16 encoding_id;  // Platform-specific encoding ID.
  u32 offset;       // Byte offset from beginning of table to the subtable for this encoding.
} __attribute__((packed));

struct OTF_CMAP_Table
{
  u16 version;     // Table version number (0).
  u16 num_tables;  // Number of encoding tables that follow.
  OTF_EncodingRecord encoding_records[];
} __attribute__((packed));

struct OTF_CMAP_SubTable_Format12
{
  u16 format;             // Subtable format; set to 12.
  u16 reserved;           // Reserved; set to 0
  u32 length;             // Byte length of this subtable (including the header)
  u32 language;           // Please see “Note on the language field in 'cmap' subtables“ in this document.
  u32 num_groups;         // Number of groupings which follow

  struct SequentialMapGroup
  {
    u32 start_char_code;  // First character code in this group
    u32 end_char_code;    // Last character code in this group
    u32 start_glyph_id;   // Glyph index corresponding to the starting character code

  } groups[];             // Array of SequentialMapGroup records.
} __attribute__((packed));


// 'head' Font header
struct OTF_HEAD_Table
{
  u16 major_version;          // Major version number of the font header table — set to 1.
  u16 minor_version;          // Minor version number of the font header table — set to 0.
  OTF_Fixed font_revision;    // Set by font manufacturer.
  u32 check_sum_adjustment;   // To compute: set it to 0, sum the entire font as u32, then store 0xB1B0AFBA - sum. If the font is used as a
                              //   component in a font collection file, the value of this field will be invalidated by changes to the file
                              //   structure and font table directory, and must be ignored.
  u32 magic_number;           // Set to 0x5F0F3CF5.
  u16 flags;                  // Bit 0: Baseline for font at y=0;
                              // Bit 1: Left sidebearing point at x=0 (relevant only for TrueType rasterizers) — see the note below
                              //   regarding variable fonts;
                              // Bit 2: Instructions may depend on point size;
                              // Bit 3: Force ppem to integer values for all internal scaler math; may use fractional ppem sizes if this
                              //   bit is clear;
                              // Bit 4: Instructions may alter advance width (the advance widths might not scale linearly);
                              // Bit 5: This bit is not used in OpenType, and should not be set in order to ensure compatible behavior on
                              //   all platforms. If set, it may result in different behavior for vertical layout in some platforms. (See
                              //   Apple's specification for details regarding behavior in Apple platforms.)
                              // Bits 6–10: These bits are not used in Opentype and should always be cleared. (See Apple's specification
                              //   for details regarding legacy used in Apple platforms.)
                              // Bit 11: Font data is “lossless” as a results of having been subjected to optimizing transformation and/or
                              //   compression (such as e.g. compression mechanisms defined by ISO/IEC 14496-18, MicroType Express,
                              //   WOFF 2.0 or similar) where the original font functionality and features are retained but the binary
                              //   compatibility between input and output font files is not guaranteed. As a result of the applied
                              //   transform, the 'DSIG' table may also be invalidated.
                              // Bit 12: Font converted (produce compatible metrics)
                              // Bit 13: Font optimized for ClearType™. Note, fonts that rely on embedded bitmaps (EBDT) for rendering
                              //   should not be considered optimized for ClearType, and therefore should keep this bit cleared.
                              // Bit 14: Last Resort font. If set, indicates that the glyphs encoded in the cmap subtables are simply
                              //   generic symbolic representations of code point ranges and don’t truly represent support for those code
                              //   points. If unset, indicates that the glyphs encoded in the cmap subtables represent proper support for
                              //   those code points.
                              // Bit 15: Reserved, set to 0
  u16 units_per_em;           // Set to a value from 16 to 16384. Any value in this range is valid. In fonts that have TrueType outlines, a
                              //   power of 2 is recommended as this allows performance optimizations in some rasterizers.
  OTF_LongDateTime created;   // Number of seconds since 12:00 midnight that started January 1st 1904 in GMT/UTC time zone. 64-bit integer
  OTF_LongDateTime modified;  // Number of seconds since 12:00 midnight that started January 1st 1904 in GMT/UTC time zone. 64-bit integer
  s16 x_min;                  // For all glyph bounding boxes.
  s16 y_min;                  // For all glyph bounding boxes.
  s16 x_max;                  // For all glyph bounding boxes.
  s16 y_max;                  // For all glyph bounding boxes.
  u16 mac_style;              // Bit 0: Bold (if set to 1);
                              // Bit 1: Italic (if set to 1)
                              // Bit 2: Underline (if set to 1)
                              // Bit 3: Outline (if set to 1)
                              // Bit 4: Shadow (if set to 1)
                              // Bit 5: Condensed (if set to 1)
                              // Bit 6: Extended (if set to 1)
                              // Bits 7–15: Reserved (set to 0).
  u16 lowest_rec_PPEM;        // Smallest readable size in pixels.
  s16 font_direction_hint;    // Deprecated (Set to 2).
  s16 index_to_loc_format;    // 0 for short offsets (Offset16), 1 for long (Offset32).
  s16 glyph_data_format;      // 0 for current format.
} __attribute__((packed));


// 'hhea' Horizontal header
struct OTF_HHEA_Table
{
  u16 major_version;                 // Major version number of the horizontal header table — set to 1.
  u16 minor_version;                 // Minor version number of the horizontal header table — set to 0.
  OTF_FWord ascender;                // Typographic ascent (Distance from baseline of highest ascender).
  OTF_FWord descender;               // Typographic descent (Distance from baseline of lowest descender).
  OTF_FWord line_gap;                // Typographic line gap.
                                     // Negative LineGap values are treated as zero in Windows 3.1, and in Mac OS System 6 and System 7.
  OTF_uFWord advance_width_max;      // Maximum advance width value in 'hmtx' table.
  OTF_FWord min_left_side_bearing;   // Minimum left sidebearing value in 'hmtx' table.
  OTF_FWord min_right_side_bearing;  // Minimum right sidebearing value; calculated as Min(aw - lsb - (xMax - xMin)).
  OTF_FWord x_max_extent;            // Max(lsb + (xMax - xMin)).
  s16 caret_slope_rise;              // Used to calculate the slope of the cursor (rise/run); 1 for vertical.
  s16 caret_slope_run;               // 0 for vertical.
  s16 caret_offset;                  // The amount by which a slanted highlight on a glyph needs to be shifted to produce the best
                                     //   appearance. Set to 0 for non-slanted fonts
  s32 _1;                            // set to 0
  s32 _2;                            // set to 0
  s16 metric_data_format;            // 0 for current format.
  u16 number_of_h_metrics;           // Number of hMetric entries in 'hmtx' table
} __attribute__((packed));


// 'hmtx' Horizontal metrics
struct OTF_LongHorMetric
{
  u16 advance_width;       // Advance width, in font design units.
  s16 lsb;                 // Glyph left side bearing, in font design units.
} __attribute__((packed));

struct OTF_HMTX_Table
{

} __attribute__((packed));

struct OTF_HMTX_Table_Ptrs
{
  OTF_LongHorMetric *h_metrics;  // [number_of_h_metrics]  Paired advance width and left side bearing values for each glyph. Records are
                                 //   indexed by glyph ID.
  s16 *left_side_bearings;       // [numGlyphs - number_of_h_metrics]  Left side bearings for glyph IDs greater than or equal to
                                 //   number_of_h_metrics.
};


// 'maxp' Maximum profile
struct OTF_MAXP_Table
{
  OTF_Fixed version;             // 0x00010000 for version 1.0.
  u16 num_glyphs;                // The number of glyphs in the font.
  u16 max_points;                // Maximum points in a non-composite glyph.
  u16 max_contours;              // Maximum contours in a non-composite glyph.
  u16 max_composite_points;      // Maximum points in a composite glyph.
  u16 max_composite_contours;    // Maximum contours in a composite glyph.
  u16 max_zones;                 // 1 if instructions do not use the twilight zone (Z0), or 2 if instructions do use Z0; should be set to 2
                                 //   in most cases.
  u16 max_twilight_points;       // Maximum points used in Z0.
  u16 max_storage;               // Number of Storage Area locations.
  u16 max_function_defs;         // Number of FDEFs, equal to the highest function number + 1.
  u16 max_instruction_defs;      // Number of IDEFs.
  u16 max_stack_elements;        // Maximum stack depth2.
  u16 max_size_of_instructions;  // Maximum byte count for glyph instructions.
  u16 max_component_elements;    // Maximum number of components referenced at “top level” for any composite glyph.
  u16 max_component_depth;       // Maximum levels of recursion; 1 for simple components.
} __attribute__((packed));


// 'name' Naming table
struct OTF_NAME_Table
{
  // Not being used.
} __attribute__((packed));


// 'OS/2' OS/2 and Windows specific metrics
struct OTF_OS_2_Table
{
  u16 version;                   // 0x0005
  s16 x_avg_char_width;          // The Average Character Width parameter specifies the arithmetic average of the escapement (width) of all
                                 //   non-zero width glyphs in the font.
  u16 weight_class;
  u16 width_class;
  u16 fs_type;
  s16 y_subscript_x_size;
  s16 y_subscript_y_size;
  s16 y_subscript_x_offset;
  s16 y_subscript_y_offset;
  s16 y_superscript_x_size;
  s16 y_superscript_y_size;
  s16 y_superscript_x_offset;
  s16 y_superscript_y_offset;
  s16 y_strikeout_size;
  s16 y_strikeout_position;
  s16 family_class;
  u8 panose[10];
  u32 unicode_range_1;           // https://www.microsoft.com/typography/otspec/os2.htm#ur
  u32 unicode_range_2;
  u32 unicode_range_3;
  u32 unicode_range_4;
  u8 ach_vend_id[4];
  u16 fs_selection;
  u16 first_char_index;
  u16 last_char_index;
  s16 typo_ascender;
  s16 typo_descender;
  s16 typo_line_gap;
  u16 win_ascent;
  u16 win_descent;
  u32 code_page_range_1;         // Bits 0–31
  u32 code_page_range_2;         // Bits 32–63
  s16 x_height;
  s16 cap_height;
  u16 default_char;
  u16 break_char;
  u16 max_context;
  u16 lower_optical_point_size;
  u16 upper_optical_point_size;
} __attribute__((packed));


// 'post' PostScript information
struct OTF_POST_Table
{
  // Not being used.
} __attribute__((packed));


struct OTF_Tables
{
  OTF_CMAP_Table *cmap;
  OTF_HEAD_Table *head;
  OTF_HHEA_Table *hhea;
  OTF_HMTX_Table *hmtx;
  OTF_MAXP_Table *maxp;
  OTF_NAME_Table *name;
  OTF_OS_2_Table *os_2;
  OTF_POST_Table *post;
};


// TrueType tables:

// 'cvt ' Control Value Table (optional table)
struct OTF_CVT__Table
{
  OTF_FWord values[];  // List of n values referenceable by instructions. n is the number of OTF_FWord items that fit in the size of the
                       //   table.
} __attribute__((packed));


// 'fpgm' Font program (optional table)
struct OTF_FPGM_Table
{
  u8 instructions[];
} __attribute__((packed));


// 'glyf' Glyph data
struct OTF_GLYF_Header
{
  s16 number_of_contours;  // If the number of contours is greater than or equal to zero, this is a single glyph. If negative, this is a
                           //   composite glyph — the value -1 should be used for composite glyphs.
  s16 x_min;               // Minimum x for coordinate data.
  s16 y_min;               // Minimum y for coordinate data.
  s16 x_max;               // Maximum x for coordinate data.
  s16 y_max;               // Maximum y for coordinate data.
} __attribute__((packed));

struct OTF_GLYF_Table
{
  OTF_GLYF_Header glyphs[];
} __attribute__((packed));


enum OTF_GLYF_SimpleFlags : u8
{
  OTF_GLYF_FLAG_ON_CURVE_POINT                       = 0x01,  // Bit 0: If set, the point is on the curve; otherwise, it is off the curve.
  OTF_GLYF_FLAG_X_SHORT_VECTOR                       = 0x02,  // Bit 1: If set, the corresponding x-coordinate is 1 byte long. If not set, it is two bytes long. For the sign of this value, see the description of the X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR flag.
  OTF_GLYF_FLAG_Y_SHORT_VECTOR                       = 0x04,  // Bit 2: If set, the corresponding y-coordinate is 1 byte long. If not set, it is two bytes long. For the sign of this value, see the description of the Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR flag.
  OTF_GLYF_FLAG_REPEAT_FLAG                          = 0x08,  // Bit 3: If set, the next byte (read as unsigned) specifies the number of additional times this flag is to be repeated — that is, the number of additional logical flag entries inserted after this entry. In this way, the number of flags listed can be smaller than the number of points in the glyph description.
  OTF_GLYF_FLAG_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR = 0x10,  // Bit 4: This flag has two meanings, depending on how the X_SHORT_VECTOR flag is set. If X_SHORT_VECTOR is set, this bit describes the sign of the value, with 1 equalling positive and 0 negative. If X_SHORT_VECTOR is not set and this bit is set, then the current x-coordinate is the same as the previous x-coordinate. If X_SHORT_VECTOR is not set and this bit is also not set, the current x-coordinate is a signed 16-bit delta vector.
  OTF_GLYF_FLAG_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR = 0x20,  // Bit 5: This flag has two meanings, depending on how the Y_SHORT_VECTOR flag is set. If Y_SHORT_VECTOR is set, this bit describes the sign of the value, with 1 equalling positive and 0 negative. If Y_SHORT_VECTOR is not set and this bit is set, then the current y-coordinate is the same as the previous y-coordinate. If Y_SHORT_VECTOR is not set and this bit is also not set, the current y-coordinate is a signed 16-bit delta vector.
  OTF_GLYF_FLAG_RESERVED                             = 0xC0,  // Bits 6 and 7 are reserved: set to zero.
};


// Simple Glyph Table
struct OTF_GLYF_Simple_Ptrs
{
  u16 number_of_contours;
  u16 *end_points_of_contours;             // numberOfContours; Array of point indices for the last point of each contour, in increasing numeric order.

  u16 instruction_length;                  // Total number of bytes for instructions. If instructionLength is zero, no instructions are present for
                                           //   this glyph, and this field is followed directly by the flags field.
  u8 *instructions;                        // instruction_length; Array of instruction byte code for the glyph.

  u32 n_points;

  OTF_GLYF_SimpleFlags *coordinate_flags;  // Array of flags for each point in the outline.
  u8 *x_coordinates;                       // Contour point x-coordinates.
  u8 *y_coordinates;                       // Contour point y-coordinates.
                                           // Coordinate for the first point is relative to (0,0); others are relative to previous point.
};


struct GlyphLocation
{
  u32 start_position;
  u32 end_position;
};


// 'loca' Index to location
struct OTF_LOCA_Table
{
  union
  {
    u16 offsets_short[];  // The actual local offset divided by 2 is stored. The value of n is numGlyphs + 1. The value for numGlyphs is
                          //   found in the 'maxp' table.
    u32 offsets_long[];   // The actual local offset is stored. The value of n is numGlyphs + 1. The value for numGlyphs is found in the
                          //   'maxp' table.
  };
} __attribute__((packed));


// 'prep' CVT Program (optional table)
struct OTF_PREP_Table
{
  u8 instructions[];  // Set of instructions executed whenever point size or font or transformation change.
} __attribute__((packed));


// 'gasp' Grid-fitting/Scan-conversion (optional table)
enum OTF_GaspRangeBehavior : u16
{
  OTF_GASP_GRIDFIT             = 0x0001,  // Use gridfitting
  OTF_GASP_DOGRAY              = 0x0002,  // Use grayscale rendering
  OTF_GASP_SYMMETRIC_GRIDFIT   = 0x0004,  // Use gridfitting with ClearType symmetric smoothing. Only supported in version 1 gasp
  OTF_GASP_SYMMETRIC_SMOOTHING = 0x0008,  // Use smoothing along multiple axes with ClearType®. Only supported in version 1 gasp
};

struct OTF_GaspRange
{
  u16 range_max_PPEM;       // Upper limit of range, in PPEM
  OTF_GaspRangeBehavior range_gasp_behavior;  // Flags describing desired rasterizer behavior.
} __attribute__((packed));

struct OTF_GASP_Table
{
  u16 version;                  // Version number (set to 1)
  u16 num_ranges;               // Number of records to follow
  OTF_GaspRange gasp_ranges[];  // Sorted by ppem
} __attribute__((packed));


struct OTF_TrueType_Tables
{
  OTF_CVT__Table *cvt_;
  OTF_FPGM_Table *fpgm;
  OTF_GLYF_Table *glyf;
  OTF_LOCA_Table *loca;
  OTF_PREP_Table *prep;
  OTF_GASP_Table *gasp;
};


// CFF Tables

// 'CFF ' Compact Font Format 1.0
struct OTF_CFF__Table
{
  //
} __attribute__((packed));


// 'CFF2' Compact Font Format 2.0
struct OTF_CFF2_Table
{
  //
} __attribute__((packed));


// 'VORG' Vertical Origin (optional table)
struct OTF_VORG_Table
{
  //
} __attribute__((packed));


struct OTF_CFF_Tables
{
  OTF_CFF__Table *cff_;
  OTF_CFF2_Table *cff2;
  OTF_VORG_Table *vorg;
};


// This header is for font collection files, otherwise the file starts with the OffsetTable
struct TTC_Header
{
  u8 ttc_tag[4];       // Font Collection ID string: 'ttcf'
  u16 major_version;   // Major version of the TTC Header, = 2.
  u16 minor_version;   // Minor version of the TTC Header, = 0.
  u32 num_fonts;       // Number of fonts in TTC
  u32 offset_table[];  // Array of offsets to the OffsetTable for each font from the beginning of the file
} __attribute__((packed));


struct TTC_DSIG
{
  u8 dsig_tag[4];   // Tag indicating that a DSIG table exists, 0x44534947 ('DSIG') (null if no signature)
  u32 dsig_length;  // The length (in bytes) of the DSIG table (null if no signature)
  u32 dsig_offset;  // The offset (in bytes) of the DSIG table from the beginning of the TTC file (null if no signature)
} __attribute__((packed));


struct OTF_TableRecord
{
  u8 tag[4];
  u32 check_sum;  // CheckSum for this table.
  u32 offset;     // Offset from beginning of TrueType font file.
  u32 length;     // Length of this table.
} __attribute__((packed));


struct OTF_OffsetTable
{
  u8 sfnt_version[4];  // 0x00010000 for True type outlines or 0x4F54544F ('OTTO') for CFF data
  u16 num_tables;      // number of tables
  u16 search_range;    // (maximum power of 2 <= numTables)*16
  u16 entry_selector;  // log2(maximum power of 2 <= numTables)
  u16 range_shift;     // numTables*16-searchRange

  OTF_TableRecord table_records[];
} __attribute__((packed));


enum OTF_SfntVersion
{
  OTF_SFNT_TrueType_Outlines,
  OTF_SFNT_CFF_Data,
  OTF_SFNT_Undefined
} __attribute__((packed));


struct Font
{
  File *file;
  OTF_OffsetTable *otf;

  OTF_Tables table_ptrs;

  OTF_SfntVersion sfnt_version;
  union
  {
    OTF_TrueType_Tables true_type_table_ptrs;
    OTF_CFF_Tables cff_table_ptrs;
  };

  OTF_CMAP_SubTable_Format12 *cmap_subtable_12;
};