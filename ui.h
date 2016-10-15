struct Menu
{
  u32 length;
  V2 pos;

  struct MenuItem
  {
    char name[64];
  } *items;
};

const u32 MAX_MENUS = 32;


struct UI
{
  Menu menu_items[MAX_MENUS];
};


const char *CellTypeNames[] =
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