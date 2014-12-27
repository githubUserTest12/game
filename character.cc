#include "character.hpp"

Dot::Dot() {
	//Initialize the collision box
	mPosX = DOT_WIDTH + TILE_WIDTH;
	mPosY = 0;

	mBox.x = 0;
	mBox.y = 0;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;
	isJumping = false;
	isMoving = false;
	flip = SDL_FLIP_NONE;

	//Load dot texture
	if(!dotTexture.loadFromFile("character.png")) {
		printf("Failed to load dot texture!\n");
	}
	else {
		int x = 0; 
		int y = 0;
		for(int i = 0; i < ANIMATION_FRAMES; ++i) {
			spriteClips[i].x = x;
			spriteClips[i].y = y;
			spriteClips[i].w = DOT_WIDTH;
			spriteClips[i].h = DOT_HEIGHT;

			x += DOT_WIDTH + 1;
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
	for(int i = 0; i < TOTAL_PARTICLES; ++i) {
		particles[i] = new Particle(mPosX, mPosY, mBox);
	}

}

void Dot::renderParticles(SDL_Rect &camera, bool toggleParticles) {
	if(toggleParticles) {
		// Go through particles.
		for(int i = 0; i < TOTAL_PARTICLES; ++i) {

			if(particles[i] != NULL) {
				if(particles[i]->isDead()) {
					delete particles[i];
					//if(camera.x > 0 && camera.x < LEVEL_WIDTH - camera.w) {
					//std::cout << "hit " << mPosX << std::endl;
					//particles[i] = new Particle((SCREEN_WIDTH / 2) - (DOT_WIDTH / 2), mPosY - camera.y);
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

void Dot::handleEvent(SDL_Event &e) {
	//If a key was pressed
	if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		//Adjust the velocity
		switch(e.key.keysym.sym) {
			case SDLK_SPACE: 
				//if(!isJumping) {
				mVelY = 0; 
				mVelY -= DOT_VELY; 
				//std::cout << mVelY << std::endl;
				//}
				break;
			case SDLK_w: 
				mVelY -= DOT_VELY;
				//std::cout << mVelY << std::endl;
				break;
			case SDLK_s: 
				mVelY += DOT_VELY;
				//std::cout << mVelY << std::endl;
				break;
			//case SDLK_DOWN: mVelY += DOT_VELY; break;
			case SDLK_a: 
				mVelX -= DOT_VELX; 
				break;
			case SDLK_d: 
				mVelX +=  DOT_VELX; 
				break;
		}
	}
	//If a key was released
	else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
		//Adjust the velocity
		switch(e.key.keysym.sym) {
			case SDLK_w: mVelY += DOT_VELY; break;
			case SDLK_s: mVelY -= DOT_VELY; break;
			//case SDLK_DOWN: mVelY -= DOT_VELY; break;
			case SDLK_a: mVelX += DOT_VELX; break; 
			case SDLK_d: mVelX -= DOT_VELX; break; 
		}
	}
}

Dot::~Dot() {
	// Delete particles.
	for(int i = 0; i < TOTAL_PARTICLES; ++i) {
		delete particles[i];
	}
}

void Dot::move(Tile *tiles[], Npc *npcContainer[], float timeStep) {

	int tileTouched, npcTouched;

	//Move the dot left or right
	mPosX += mVelX * timeStep;

	//If the dot went too far to the left or right or touched a wall
	if((mPosX < 0) || (mPosX + DOT_WIDTH > LEVEL_WIDTH)) {
		//move back
		if(mPosX < 0) {
			mPosX = 0;
		}
		else {
			mPosX = LEVEL_WIDTH - DOT_WIDTH;
		}
	}
	mBox.x = mPosX;
	tileTouched = touchesWall(mBox, tiles);
	if(tileTouched > -1 && mVelX > 0) {
		mPosX = tiles[tileTouched]->getBox().x - DOT_WIDTH;
	}
	if(tileTouched > -1 && mVelX < 0) {
		mPosX = tiles[tileTouched]->getBox().x + TILE_WIDTH;
	}
	npcTouched = touchesNpc(mBox, npcContainer);
	if(npcTouched > -1 && mVelX > 0) {
		mPosX = npcContainer[npcTouched]->getPosX() - DOT_WIDTH;
	}
	if(npcTouched > -1 && mVelX < 0) {
		mPosX = npcContainer[npcTouched]->getPosX() + npcContainer[npcTouched]->NPC_WIDTH;
	}
	mBox.x = mPosX;

	//Move the dot up or down
	mPosY += mVelY * timeStep;

	//If the dot went too far up or down or touched a wall
	if((mPosY < 0) || (mPosY + DOT_HEIGHT > LEVEL_HEIGHT)) {
		if(mPosY < 0) mPosY = 0;
		else {
			mPosY = LEVEL_HEIGHT - DOT_HEIGHT;
			isJumping = false;
		}
	} 
	mBox.y = mPosY;
	tileTouched = touchesWall(mBox, tiles);
	if(tileTouched > -1 && mVelY > 0) {
		mPosY = tiles[tileTouched]->getBox().y - DOT_HEIGHT;
		isJumping = false;
	}
	if(tileTouched > -1 && mVelY < 0) {
		mPosY = tiles[tileTouched]->getBox().y + TILE_HEIGHT;
	}
	npcTouched = touchesNpc(mBox, npcContainer);
	if(npcTouched > -1 && mVelY > 0) {
		mPosY = npcContainer[npcTouched]->getPosY() - DOT_HEIGHT;
		isJumping = false;
	}
	if(npcTouched > -1 && mVelY < 0) {
		mPosY = npcContainer[npcTouched]->getPosY() + npcContainer[npcTouched]->NPC_HEIGHT;
	}
	mBox.y = mPosY;
}

void Dot::setCamera(SDL_Rect &camera) {
	//Center the camera over the dot
	camera.x = (mPosX + DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera.y = (mPosY + DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if(camera.x < 0) {
		camera.x = 0;
	}
	if(camera.y < 0) {
		camera.y = 0;
	}
	if(camera.x > LEVEL_WIDTH - camera.w) {
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if(camera.y > LEVEL_HEIGHT - camera.h) {
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void Dot::render(SDL_Rect &camera, bool toggleParticles, SDL_Rect *clip) {
	//Show the dot
	dotTexture.render((int)(mPosX) - camera.x, (int)(mPosY) - camera.y, clip, 0, NULL, flip);

	// Show particles on top of dot.
	renderParticles(camera, toggleParticles);
}
