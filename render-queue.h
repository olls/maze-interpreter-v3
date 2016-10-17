const u32 RENDER_QUEUE_SIZE = 10;


struct
RenderData
{
  GameState game_state;
  RenderBasis render_basis;
  FrameBuffer *frame_buffer;
  u64 time_us;
  u32 id;
};


struct
RenderQueue
{
  RenderData queue[RENDER_QUEUE_SIZE];
  u32 head;
  u32 tail;
  u32 full;
  u32 empty;
  pthread_mutex_t mut;
  pthread_cond_t notFull;
  pthread_cond_t notEmpty;
};


