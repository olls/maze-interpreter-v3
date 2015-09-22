
struct GameMemory
{
  uint32_t total;
  uint8_t * memory;
  uint8_t * pos;
};


struct Keys
{
  bool p_down;
  bool w_down;
  bool s_down;
  bool a_down;
  bool d_down;
};


struct Mouse
{
  uint32_t x;
  uint32_t y;

  bool l_down;
  bool r_down;
};
