
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


struct Player
{
  uint32_t x;
  uint32_t y;

  uint32_t width;
  uint32_t height;
};
