u16 *
outline_to_triangles(Memory *frame_memory, VertexArray outline, u32 *n_indices)
{
  // Triangulation by Ear-Clipping

  u32 n_vertices = outline.n_vertices;
  u32 n_triangles = n_vertices - 2;
  *n_indices = n_triangles * 3;

  u16 *resulting_indices = push_structs(frame_memory, u16, *n_indices);
  u32 result_position = 0;

  assert(n_vertices < MAX_U16);

  if (n_vertices < 3)
  {
    log(L_Font, u8("Error in outline_to_triangles, not enough vertices supplied to form minimum of one triangle."));
    *n_indices = 0;
    resulting_indices = 0;
  }
  else if (n_triangles == 1)
  {
    // Outline is a single triangle
    resulting_indices[0] = 0;
    resulting_indices[1] = 1;
    resulting_indices[2] = 2;
  }
  else
  {
    // Put vertices into doubly linked list

    DoublyLinked_Vertex *doubly_linked_vertices = push_structs(frame_memory, DoublyLinked_Vertex, *n_indices);

    DoublyLinked_Vertex *previous = doubly_linked_vertices + n_vertices - 1;
    for (u32 vertex_n = 0;
         vertex_n < n_vertices;
         ++vertex_n)
    {
      DoublyLinked_Vertex *vertex = doubly_linked_vertices + vertex_n;

      vertex->v = outline.vertices[vertex_n];
      vertex->index = vertex_n;

      vertex->previous = previous;
      previous->next = vertex;

      previous = vertex;
    }

    // Find ears recursively

    u32 vertices_left = n_vertices;

    DoublyLinked_Vertex *first_vertex = doubly_linked_vertices;

    while (result_position < *n_indices)
    {
      DoublyLinked_Vertex *vertex = first_vertex;
      b32 found_an_ear = false;

      for (u32 vertex_n = 0;
           vertex_n < vertices_left;
           ++vertex_n)
      {
        // Is vertex an ear tip?

        DoublyLinked_Vertex *previous = vertex->previous;
        DoublyLinked_Vertex *next = vertex->next;

        vec2 test_ear_0 = previous->v;
        vec2 test_ear_1 = vertex->v;
        vec2 test_ear_2 = next->v;

        // Are there any other vertices inside this triangle?
        b32 is_ear = true;

        DoublyLinked_Vertex *test_vertex = first_vertex;

        for (u32 vertex_test_n = 0;
             vertex_test_n < vertices_left;
             ++vertex_test_n)
        {
          if (test_vertex->index != previous->index &&
              test_vertex->index != vertex->index &&
              test_vertex->index != next->index &&
              in_triangle(test_vertex->v, test_ear_0, test_ear_1, test_ear_2))
          {
            is_ear = false;
            break;
          }

          test_vertex = test_vertex->next;
        }

        if (is_ear)
        {
          // Remove ear tip
          vertices_left -= 1;
          next->previous = previous;
          previous->next = next;

          if (first_vertex == vertex)
          {
            first_vertex = next;
          }

          // Add ear to triangles
          resulting_indices[result_position++] = previous->index;
          resulting_indices[result_position++] = vertex->index;
          resulting_indices[result_position++] = next->index;

          found_an_ear = true;
          break;
        }

        vertex = vertex->next;
      }

      if (!found_an_ear)
      {
        log(L_Font, u8("Error: after searching all vertices, no ears where found!"));

        resulting_indices = 0;
        *n_indices = 0;

        break;
      }
    }

  }

  return resulting_indices;
}