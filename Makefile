CC         = clang++
CFLAGS     = -Werror -g -ffunction-sections -ferror-limit=1 -o0
LIBS       = -lSDL2
SOURCES    = main.cpp
EXECUTABLE = maze-interpreter


all:
	$(CC) $(CFLAGS) $(SOURCES) $(LIBS) -o $(EXECUTABLE)


clean:
	find . -name '*.o' -type f -delete
	rm $(EXECUTABLE)
