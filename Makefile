compile:
	gcc game.c -o game.elf -lm -lGL -lGLU -l:libglut.so.3 `pkg-config --cflags --libs sdl2`
