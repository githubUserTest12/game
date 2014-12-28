#include "npc.hpp"
#include <iostream>

void Npc::render(SDL_Rect &camera, bool toggleParticles, SDL_Rect *clip) {
	//Show the dot
	if(checkCollision(camera, mBox)) {
		npcTexture.render(mBox.x - camera.x, mBox.y - camera.y, clip, 0, NULL, flip);
	}

	// Show particles on top of dot.
	//renderParticles(camera, toggleParticles);
}

Npc::Npc(int x, int y, int width, int height, int maxFrames, std::string filename) : NPC_WIDTH(width), NPC_HEIGHT(height), ANIMATION_FRAMES(maxFrames), SPRITESHEET_WIDTH(width * maxFrames) {
	//Initialize the collision box
	mPosX = x;
	mPosY = y;

	mBox.x = 0;
	mBox.y = 0;
	mBox.w = NPC_WIDTH;
	mBox.h = NPC_HEIGHT;
	isJumping = false;
	isMoving = false;
	flip = SDL_FLIP_NONE;
	spriteClips.resize(maxFrames);

	//Load dot texture
	if(!npcTexture.loadFromFile(filename)) {
		printf("Failed to load dot texture!\n");
	}
	else {
		int x = 0; 
		int y = 0;
		for(int i = 0; i < ANIMATION_FRAMES; ++i) {
			spriteClips[i].x = x;
			spriteClips[i].y = y;
			spriteClips[i].w = NPC_WIDTH;
			spriteClips[i].h = NPC_HEIGHT;

			x += NPC_WIDTH + 1;
			if(x >=  SPRITESHEET_WIDTH) {
				x = 0;
				y += TILE_HEIGHT;
			}
		}
	}

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	// Initialize the particles.
	/*
	for(int i = 0; i < TOTAL_PARTICLES; ++i) {
		particles[i] = new Particle(mPosX, mPosY, mBox);
	}
	*/
}

Npc::~Npc() {
	std::cout << "hi mom" << std::endl;
	npcTexture.free();
}

void Npc::move(Tile *tiles[], float timeStep) {

	int tileTouched;

	//Move the dot left or right
	mPosX += mVelX * timeStep;

	//If the dot went too far to the left or right or touched a wall
	if((mPosX < 0) || (mPosX + NPC_WIDTH > LEVEL_WIDTH)) {
		//move back
		if(mPosX < 0) {
			mPosX = 0;
		}
		else {
			mPosX = LEVEL_WIDTH - NPC_WIDTH;
		}
	}
	mBox.x = mPosX;
	tileTouched = touchesWall(mBox, tiles);
	if(tileTouched > -1 && mVelX > 0) {
		mPosX = tiles[tileTouched]->getBox().x - NPC_WIDTH;
	}
	if(tileTouched > -1 && mVelX < 0) {
		mPosX = tiles[tileTouched]->getBox().x + TILE_WIDTH;
	}
	mBox.x = mPosX;

	//Move the dot up or down
	mPosY += mVelY * timeStep;

	//If the dot went too far up or down or touched a wall
	if((mPosY < 0) || (mPosY + NPC_HEIGHT > LEVEL_HEIGHT)) {
		if(mPosY < 0) mPosY = 0;
		else {
			mPosY = LEVEL_HEIGHT - NPC_HEIGHT;
			isJumping = false;
		}
	} 
	mBox.y = mPosY;
	tileTouched = touchesWall(mBox, tiles);
	if(tileTouched > -1 && mVelY > 0) {
		mPosY = tiles[tileTouched]->getBox().y - NPC_HEIGHT;
		isJumping = false;
	}
	if(tileTouched > -1 && mVelY < 0) {
		mPosY = tiles[tileTouched]->getBox().y + TILE_HEIGHT;
	}
	mBox.y = mPosY;
}

/*
void Npc::renderParticles(SDL_Rect &camera, bool toggleParticles) {
	if(toggleParticles) {
		// Go through particles.
		for(int i = 0; i < TOTAL_PARTICLES; ++i) {

			if(particles[i] != NULL) {
				if(particles[i]->isDead()) {
					delete particles[i];
					//if(camera.x > 0 && camera.x < LEVEL_WIDTH - camera.w) {
					//std::cout << "hit " << mPosX << std::endl;
					//particles[i] = new Particle((SCREEN_WIDTH / 2) - (NPC_WIDTH / 2), mPosY - camera.y);
					//}
					//else 
					particles[i] = new Particle(mPosX - camera.x, mPosY - camera.y, mBox);
				}
			}
			else {
				particles[i] = new Particle(mPosX - camera.x, mPosY - camera.y, mBox);
			}
		}

		// Show particles.
		for(int i = 0; i < TOTAL_PARTICLES; ++i) {
			particles[i]->render();
		}
	}
	else {
		// Delete and replace dead particles.
		for(int i = 0; i < TOTAL_PARTICLES; ++i) {
			delete particles[i];
			particles[i] = NULL;
		}
	}
}
*/

