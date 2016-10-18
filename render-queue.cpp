void
queue_add(RenderQueue *q, RenderQueueData in)
{
  q->queue[q->tail] = in;
  q->tail++;
  if (q->tail == RENDER_QUEUE_SIZE)
  {
    q->tail = 0;
  }
  if (q->tail == q->head)
  {
    q->full = true;
  }
  q->empty = false;
}


void
queue_del(RenderQueue *q, RenderQueueData *out)
{
  *out = q->queue[q->head];

  q->head++;
  if (q->head == RENDER_QUEUE_SIZE)
  {
    q->head = 0;
  }
  if (q->head == q->tail)
  {
    q->empty = true;
  }
  q->full = false;
}
