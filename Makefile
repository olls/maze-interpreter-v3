CC         = clang++
CFLAGS     = -Werror -g -ferror-limit=1 -Ofast
LIBS       = -lSDL2 -lGLEW -lGL -lGLU -lpthread -lfreetype -I/usr/include/freetype2
SOURCES    = main.cpp
EXECUTABLE = maze-interpreter


all:
	$(CC) $(CFLAGS) $(SOURCES) $(LIBS) -o $(EXECUTABLE)


clean:
	find . -name '*.o' -type f -delete
	rm $(EXECUTABLE)
