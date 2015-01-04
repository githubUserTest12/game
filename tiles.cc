#include "tiles.hpp"
#include "globals.hpp"
#include "iostream"

Tile::Tile(int x, int y, int tileType) {
	//Get the offsets
	mBox.x = x;
	mBox.y = y;

	//Set the collision box
	mBox.w = TILE_WIDTH;
	mBox.h = TILE_HEIGHT;

	//Get the tile type
	if(tileType % 4 != 0 && tileType > 20 && tileType <= 27 && tileType != 23) {
		topHalf = true;
		mCollisionBox = mBox;
		mCollisionBox.h = (TILE_HEIGHT / 2);
	}
	if(tileType == 48) {
		diagonalTile = true;
		int tmp = 0;
		mPixelBox.resize(80);
		/*
		mPixelBox[tmp] = mBox;
		mPixelBox[tmp].y = mBox.y + 40;
		*/
		for(int i = 79; i >= 0; --i) {
			std::cout << tmp << ", x: "  << mBox.x + i << " y: " << mBox.y + (79 - i) << std::endl;
			mPixelBox[tmp].x = mBox.x + i;
			mPixelBox[tmp].y = mBox.y + (79 - i);
			mPixelBox[tmp].w = 1;
			mPixelBox[tmp++].h = 1;
		}
	}
	mType = tileType;
}

void Tile::render(SDL_Rect& camera) {
	//If the tile is on screen
	if(checkCollision(camera, mBox)) {
		//Show the tile
		gTileTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips[mType]);
	}
}

int Tile::getType() {
	return mType;
}

SDL_Rect Tile::getBox() {
	return mBox;
}

SDL_Rect Tile::getCollisionBox() {
	return mCollisionBox;
}

std::vector<SDL_Rect> &Tile::getPixelBox() {
	return mPixelBox;
}
