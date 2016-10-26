const V4 FRAME_COLOR = {1, 0.35, 0.5, .95};
const r32 FONT_SIZE = 0.2;
const r32 MENU_ANNIMATION_SPEED = .4;
const V2 MENU_ITEM_SIZE = {250, 25};
const u32 MAX_UI_BOX_CHARS = 32;


struct MenuItem
{
  char name[MAX_UI_BOX_CHARS];
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
  b32 clicked;

  MenuItem items[MAX_MENU_ITEMS];
  u32 length;
  u32 chars_wide;
  V2 pos;

  MenuItemSelector hover_selector;
  MenuItemSelector selected_selector;
  u64 opened_on_frame;

  V2 annimated_highlighted_cell_pos;
};


struct InputBox
{
  char text[MAX_UI_BOX_CHARS];
  u32 cursor_pos;
  u32 length;
  V2 pos;
  b32 active;
  b32 allow_num;
  b32 allow_alpha;
  b32 allow_all;
};


struct Button
{
  V2 pos;
  u32 length;
  char name[MAX_UI_BOX_CHARS];
  u64 hovered_at_time;
  b32 activated;
};


struct CarInput
{
  InputBox input;
  Button done;
  u32 car_id;

  CarInput *next;
};


const u32 MAX_TEXT_INPUTS = 16;
struct UI
{
  Menu cell_type_menu;

  CarInput *car_inputs;
  CarInput *free_car_inputs;
};