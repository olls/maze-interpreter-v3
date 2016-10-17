void
queue_add(RenderQueue *q, RenderData in)
{
  q->queue[q->tail] = in;
  q->tail++;
  if (q->tail == RENDER_QUEUE_SIZE)
  {
    q->tail = 0;
  }
  if (q->tail == q->head)
  {
    q->full = 1;
  }
  q->empty = 0;
}


void
queue_del(RenderQueue *q, RenderData *out)
{
  *out = q->queue[q->head];

  q->head++;
  if (q->head == RENDER_QUEUE_SIZE)
  {
    q->head = 0;
  }
  if (q->head == q->tail)
  {
    q->empty = 1;
  }
  q->full = 0;
}
