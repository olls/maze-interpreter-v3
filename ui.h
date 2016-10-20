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

  s32 annimated_hover;
  V2 annimated_hover_pos;
  s32 annimated_selected;
  V2 annimated_selected_pos;
};


struct UI
{
  Menu cell_type_menu;
  Cell *cell_currently_being_edited;
};