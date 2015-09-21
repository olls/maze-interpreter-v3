CC         = clang++
CFLAGS     = -Werror -g -ffunction-sections -ferror-limit=1
LIBS       = -lSDL2
SOURCES    = main.cpp
EXECUTABLE = thingy


all:
	$(CC) $(CFLAGS) $(SOURCES) $(LIBS) -o $(EXECUTABLE)


clean:
	find . -name '*.o' -type f -delete
	rm $(EXECUTABLE)
