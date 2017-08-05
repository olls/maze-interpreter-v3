Rectangle *
add_box(Layouter *layouter, vec2 desired_pos, vec2 desired_size)
{
  Rectangle *result = layouter->rects + layouter->next_free++;
  result->start = desired_pos;
  result->end = result->start + desired_size;
  return result;
}


void
layout_boxes(Layouter *layouter)
{
  log_s(L_Layouter, u8("init\n"));
  u32 passes = 0;
  b32 moved_rect = true;
  while (moved_rect && passes < MAX_PASSES)
  {
    log_s(L_Layouter, u8(">\n"));
    moved_rect = false;

    for (u32 subject_index = 0;
         subject_index < layouter->next_free;
         ++subject_index)
    {
      log_s(L_Layouter, u8("  "));
      Rectangle *current_subject = layouter->rects + subject_index;

      for (u32 test_index = 0;
           test_index < layouter->next_free;
           ++test_index)
      {
        Rectangle *test = layouter->rects + test_index;

        if (test != current_subject)
        {
          if (overlaps(*current_subject, *test))
          {
            log_s(L_Layouter, u8("  "));
            moved_rect = true;
            Rectangle overlap = get_overlap(*current_subject, *test);

            r32 h_dist = overlap.end.x - overlap.start.x;
            r32 v_dist = overlap.end.y - overlap.start.y;

            vec2 direction = vector_direction_or_1(get_center(*test) - get_center(*current_subject));

            if (h_dist < v_dist)
            {
              *test += (vec2){h_dist, 0} * direction.x;
            }
            else
            {
              *test += (vec2){0, v_dist} * direction.y;
            }

          }
          else
          {
            log_s(L_Layouter, u8("# "));
          }
        }
        else
        {
          log_s(L_Layouter, u8("X "));
        }

      }
      log_s(L_Layouter, u8("\n"));
    }
    ++passes;
  }

  if (moved_rect)
  {
    log(L_Layouter, u8("Couldn't find layout in time..."));
  }
}