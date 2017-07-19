CC         = clang++
CFLAGS     = -Werror -g -ferror-limit=1 -O0 -std=c++14
LIBS       = -lSDL2 -lGLEW -lGL -lGLU -lpthread -lfreetype -I/usr/include/freetype2


.PHONY: maze-interpreter no-gui

maze-interpreter:
	$(CC) $(CFLAGS) main.cpp $(LIBS) -o maze-interpreter

no-gui:
	$(CC) $(CFLAGS) no-gui.cpp $(LIBS) -o maze-interpreter-no-gui


clean:
	find . -name '*.o' -type f -delete
	rm $(EXECUTABLE)
