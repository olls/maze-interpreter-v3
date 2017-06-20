// Rendering using VBO and IBO:
// - One shader for drawing cells, in world space (using uniforms to offset position)
//   - Have to load in and out cell vertices from VBO as the world moves around
//     - Use chunks?  Using quad map chunks?
//       - Split one big VBO into regions to support N chunks:
//         - Use hash-map client side to store chunk mapping to VBO offset
//         - When deleting
//       - Each frame:
//         - First find quad map chunks in view
//         - Then diff with list of currently loaded chunks
//         - Load new data into VBO chunks
//         - Draw each VBO chunk separately, changing offset uniform each time
//         - Instanced rendering to reduce duplicated geometry?

// We need to build the renderer under the assumption that we will be
//   rendering the entire map on the screen in one frame, even though the
//   resolution will be too low to make out individual cells in a full size
//   map zoomed out all the way, it will be nice to be able to zoom out to get
//   an overview of the map.

// To allow us to stop worrying about the precision problem: When re-centring
//   the map around the origin, the maximum position has to be half the max
//   signed integer, as this will allow us to render at one extreme, whilst
//   centred at the opposite extreme. (0.5 - -0.5 = 1).
// If we limit the maximum map size to 10000x10000, as it is the largest
//   reasonable size which will fit in memory if all positions are filled.
//   (32bytes * 10000^2 / 1024^3 ~= 3Gb)
// Then we don't have to worry about the precision problem nearly as much as
//   10000 is well below 0.5 * 2^32


struct Renderer
{
  SDL_Window *window;
  SDL_GLContext gl_context;

  u32 width;
  u32 height;
};


typedef GLint OpenGLViewport[4];


struct OpenGLProjection
{
  GLdouble modelview[16];
  GLdouble projection[16];
};


struct ShaderAttributes {
  GLint world_cell_position_x;
  GLint world_cell_position_y;
  GLfloat world_cell_offset[2];
  GLfloat colour[4];
};
