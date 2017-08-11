struct DoublyLinked_Vertex
{
  vec2 v;
  u16 index;
  DoublyLinked_Vertex *previous;
  DoublyLinked_Vertex *next;
};