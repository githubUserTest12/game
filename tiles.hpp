#ifndef TILES_HPP
	#define TILES_HPP
#include <SDL.h>
#include <vector>
//The tile
class Tile {
	public:
		//Initializes position and type
		Tile(int x, int y, int tileType);

		//Shows the tile
		void render(SDL_Rect& camera);

		//Get the tile type
		int getType();
		bool diagonalTile = false;
		bool topHalf = false;
		int pixelTouched = 0;

		//Get the collision box
		SDL_Rect getBox();
		SDL_Rect getCollisionBox();
		std::vector<SDL_Rect> &getPixelBox();

	private:
		//The attributes of the tile
		SDL_Rect mBox;

		SDL_Rect mCollisionBox;

		// Per-pixel collision box;
		std::vector<SDL_Rect> mPixelBox;

		//The tile type
		int mType;
};
#endif
