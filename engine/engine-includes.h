#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>


#include "utils.h"
#include "platform.h"
#include "files.h"
#include "print.h"
#include "text.h"
#include "logging.h"
#include "maths.h"
#include "vectors.h"
#include "matrices.h"
#include "quadratic-bezier.h"
#include "string.h"
#include "xml.h"
#include "svg.h"
#include "draw.h"
#include "bitmap.h"
#include "font.h"
#include "layouter.h"
#include "opentype.h"
#include "opengl-font.h"
#include "opengl-util.h"

#include "engine.h"

#include "utils.cpp"
#include "platform.cpp"
#include "files.cpp"
#include "print.cpp"
#include "text.cpp"
#include "logging.cpp"
#include "maths.cpp"
#include "vectors.cpp"
#include "matrices.cpp"
#include "quadratic-bezier.cpp"
#include "string.cpp"
#include "xml.cpp"
#include "svg.cpp"
#include "draw.cpp"
#include "colours.cpp"
#include "bitmap.cpp"
#include "font.cpp"
#include "layouter.cpp"
#include "opentype.cpp"
#include "opengl-font.cpp"
#include "opengl-util.cpp"

#include "engine.cpp"