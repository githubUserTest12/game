all:
	g++ *.cc -Wall -std=c++11 -lSDL2_mixer -lSDL2_ttf -lSDL2_image `sdl2-config --libs --cflags` -o game
