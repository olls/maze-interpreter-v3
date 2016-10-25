const u32 MAX_MENU_ITEM_NAME_LEN = 32;
struct MenuItem
{
  char name[MAX_MENU_ITEM_NAME_LEN];
  u64 hovered_at_time;

  u32 *some_kind_of_pointer_maybe;
  CellType cell_type;
};


struct MenuItemSelector
{
  s32 item_n;
  V2 annimated_pos;
};


const u32 MAX_MENU_ITEMS = 32;
struct Menu
{
  Cell *cell;
  V2 highlighted_cell_pos;

  MenuItem items[MAX_MENU_ITEMS];
  u32 length;
  u32 chars_wide;
  V2 pos;

  MenuItemSelector hover_selector;
  MenuItemSelector selected_selector;
  u64 opened_on_frame;
};


const u32 MAX_TEXT_INPUT = 256;
struct InputBox
{
  char text[MAX_TEXT_INPUT];
  u32 cursor_pos;
  u32 length;
  V2 pos;
  b32 allow_num;
  b32 allow_alpha;
  b32 allow_all;
};


struct UI
{
  Menu cell_type_menu;
  InputBox test_input;

  b32 mouse_click;
};