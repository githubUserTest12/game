all:
	g++ *.cc -std=c++11 -lSDL2_ttf -lSDL2_image `sdl2-config --libs --cflags`
