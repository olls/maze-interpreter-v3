const u32 RENDER_QUEUE_SIZE = 10;


struct RenderQueueData
{
  FrameBuffer *frame_buffer;
  RenderOperations *render_operations;
  Rectangle clip_region;
};


struct RenderQueue
{
  RenderQueueData queue[RENDER_QUEUE_SIZE];
  u32 head;
  u32 tail;
  b32 full;
  b32 empty;
  pthread_mutex_t mut;
  pthread_cond_t not_full;
  pthread_cond_t not_empty;
};
