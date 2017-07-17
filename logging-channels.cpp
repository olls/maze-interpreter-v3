b32
channel_enabled(GAMELoggingChannel channel)
{
  switch(channel)
  {
    case L_CarsStorage:
    case L_CarsSim:
    case L_CellsStorage:
    case L_Cells:
    case L_Parser:
    case L_Serializer:
    case L_Particles:
    case L_UI:
    case L_Render:
    case L_CellInstancing:
    case L_GameLoop:
    {
      return false;
    } break;

    default:
    {
      return true;
    } break;
  }
}
