CC         = clang++
CFLAGS     = -Werror -g -ferror-limit=1 -o0
LIBS       = -lSDL2
SOURCES    = maze-interpreter.cpp
EXECUTABLE = maze-interpreter


all:
	$(CC) $(CFLAGS) $(SOURCES) $(LIBS) -o $(EXECUTABLE)


clean:
	find . -name '*.o' -type f -delete
	rm $(EXECUTABLE)
