#include "npc.hpp"
#include <iostream>

void Npc::render(SDL_Rect &camera, bool toggleParticles, SDL_Rect *clip, float scale) {
	//Show the dot
	if(checkCollision(camera, mBox)) {
		/*
		if(scale != 1.0) {
			dstrect.w = (int) (clip->w * scale);
			dstrect.h = (int) (clip->h * scale);
		}
		else {
			dstrect.w = clip->w;
			dstrect.h = clip->h;
		}
		if(flip == SDL_FLIP_HORIZONTAL) {
			dstrect.x = (int)(mPosX) - camera.x - dstrect.w + NPC_WIDTH;
		}
		else {
			dstrect.x = (int)(mPosX) - camera.x;
		}
		dstrect.y = (int)(mPosY) - camera.y - dstrect.h + NPC_HEIGHT;
		*/
		// XXX FIXED.
		dstrect.w = (int) (currentClip->w * scale);
		dstrect.h = (int) (currentClip->h * scale);
		if(flip == SDL_FLIP_HORIZONTAL) dstrect.x = (int)(mBox.x - camera.x - dstrect.w + NPC_WIDTH);
		else dstrect.x = (int) (mBox.x - camera.x);
		dstrect.y = (int) (mBox.y - camera.y);

		if(flip == SDL_FLIP_HORIZONTAL) {
			// To adjust for clipping size.
			npcTexture.render((int)(mPosX) - camera.x - clip->w + NPC_WIDTH, (int)(mPosY) - camera.y - clip->h + NPC_HEIGHT, clip, dstrect, 0, NULL, flip);
		}
		else {
			npcTexture.render((int)(mPosX) - camera.x, (int)(mPosY) - camera.y - clip->h + NPC_HEIGHT, clip, dstrect, 0, NULL, flip);
		}
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
	currentClip = &mBox;
	isJumping = false;
	isMoving = false;
	wasStabbed = false;
	wasJumped = false;
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
			spriteClips[i].w = 38;
			spriteClips[i].h = 55;

			x += 38 + 1;
			if(x >= SPRITESHEET_WIDTH) {
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
	//std::cout << "hi mom" << std::endl;
	npcTexture.free();
}

void Npc::move(Tile *tiles[], Character &character, float timeStep) {

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
	if(checkCollision(mBox, character.getBoxPosition()) && mVelX > 0) {
		mPosX = character.getPosX() - NPC_WIDTH;
		if(character.isAttacking && character.flip == SDL_FLIP_HORIZONTAL) {
			mPosX = character.getPosX() - NPC_WIDTH;
			wasStabbed = true;
			mVelX = -15 * 60;
			wasAttackedTimer.start();
		}
	}
	else if(checkCollision(mBox, character.getBoxPosition()) && mVelX < 0) {
		mPosX = character.getPosX() + character.CHARACTER_WIDTH;
		if(character.isAttacking && character.flip == SDL_FLIP_NONE) {
			mPosX = character.getPosX() + character.CHARACTER_WIDTH;
			wasStabbed = true;
			mVelX = 15 * 60;
			wasAttackedTimer.start();
		}
	}
	mBox.x = mPosX;


	// Gravity.
	if(NPC_HEIGHT == (int) (105 * gScale)) {
		switch(rand() % 3) {
			case 0:
				mVelY += 3600 * timeStep;
				break;
			case 1:
				mVelY -= 3600 * timeStep;
				break;
			case 2:
				mVelY = 0;
				break;
		}
	}
	else if(mVelY < 900) {
		mVelY += 3600 * timeStep;
	}
	else if(mVelY > 900) {
		mVelY = 900;
	}

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
	if(checkCollision(mBox, character.getBoxPosition()) && mVelY > 0) {
		mPosY = character.getPosY() - NPC_HEIGHT;
	}
	if(checkCollision(mBox, character.getBoxPosition()) && mVelY < 0) {
		if(!character.isAttacking) mPosY = character.getPosY() + character.CHARACTER_HEIGHT;
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

