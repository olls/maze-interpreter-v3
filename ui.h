const u32 MAX_MENU_ITEM_NAME_LEN = 32;
struct MenuItem
{
  char name[MAX_MENU_ITEM_NAME_LEN];
  u64 hovered_at_time;

  u32 *some_kind_of_pointer_maybe;
  CellType cell_type;
};


const u32 MAX_MENU_ITEMS = 32;
struct Menu
{
  u32 length;
  V2 pos;
  MenuItem items[MAX_MENU_ITEMS];
};


struct UI
{
  Menu cell_type_menu;
  Cell *cell_currently_being_edited;
};


const char *CELL_TYPE_NAMES[] =
{
  "Null",                 // CELL_NULL
  "Start",                // CELL_START
  "Path",                 // CELL_PATH
  "Wall",                 // CELL_WALL
  "Hole",                 // CELL_HOLE
  "Splitter",             // CELL_SPLITTER
  "Function",             // CELL_FUNCTION
  "Once",                 // CELL_ONCE
  "Up Unless Detect",     // CELL_UP_UNLESS_DETECT
  "Down Unless Detect",   // CELL_DOWN_UNLESS_DETECT
  "Left Unless Detect",   // CELL_LEFT_UNLESS_DETECT
  "Right Unless Detect",  // CELL_RIGHT_UNLESS_DETECT
  "Inp",                  // CELL_INP
  "Out",                  // CELL_OUT
  "Up",                   // CELL_UP
  "Down",                 // CELL_DOWN
  "Left",                 // CELL_LEFT
  "Right",                // CELL_RIGHT
  "Pause"                 // CELL_PAUSE
};
