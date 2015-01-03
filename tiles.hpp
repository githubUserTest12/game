#ifndef TILES_HPP
	#define TILES_HPP
#include <SDL.h>
//The tile
class Tile {
	public:
		//Initializes position and type
		Tile(int x, int y, int tileType);

		//Shows the tile
		void render(SDL_Rect& camera);

		//Get the tile type
		int getType();

		//Get the collision box
		SDL_Rect getBox();
		SDL_Rect getCollisionBox();

	private:
		//The attributes of the tile
		SDL_Rect mBox;

		SDL_Rect mCollisionBox;

		//The tile type
		int mType;
};
#endif
