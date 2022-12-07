CC=g++
cflags=-O2 -std=c++17 -Wshadow -Wall -pedantic -lm
dbg=-fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG -g -DLOCAL
glut=-lGL -lGLU -lglut
objs=

game.out: main.cpp
	$(CC) $(cflags) $(objs) main.cpp -o game.out $(glut)

run: game.out
	./game.out
