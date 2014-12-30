#include "character.hpp"
#include <iostream>

Character::Character() {
	//Initialize the collision box
	mPosX = CHARACTER_WIDTH + TILE_WIDTH;
	mPosY = 0;

	mBox.x = 0;
	mBox.y = 0;
	mBox.w = CHARACTER_WIDTH;
	mBox.h = CHARACTER_HEIGHT;
	isJumping = false;
	isMoving = false;
	isAttacking = false;
	headJump = false;
	swordReach = false;
	flip = SDL_FLIP_NONE;
	currentFrame = 0;

	//Load character texture
	if(!characterTexture.loadFromFile("character4.png")) {
		printf("Failed to load character texture!\n");
	}
	else {
		int x = 0; 
		int y = 0;
		for(int i = 0; i < ANIMATION_FRAMES; ++i) {
			spriteClips[i].x = x;
			spriteClips[i].y = y;
			spriteClips[i].w = CHARACTER_WIDTH;
			spriteClips[i].h = CHARACTER_HEIGHT;

			x += CHARACTER_WIDTH + 1;
			if(x >=  SPRITESHEET_WIDTH) {
				x = 0;
				y += CHARACTER_HEIGHT;
			}
		}
		if(ATTACKING_FRAMES > 0) {
			x = 0;
			y = 106;
			for(int i =	ANIMATION_FRAMES; i < TOTAL_FRAMES; ++i) {
				spriteClips[i].x = x;
				spriteClips[i].y = y;
				spriteClips[i].w = 38;
				spriteClips[i].h = 55;

				x += CHARACTER_WIDTH + 1;
				/*
				if(x >= SPRITESHEET_WIDTH) {
					x = 0;
					y += CHARACTER_HEIGHT;
				}
				*/
			}
		}
	}
	currentClip = &spriteClips[0];

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	// Initialize the particles.
	for(int i = 0; i < TOTAL_PARTICLES; ++i) {
		particles[i] = new Particle(mPosX, mPosY, mBox);
	}

}

void Character::renderParticles(SDL_Rect &camera, bool toggleParticles) {
	if(toggleParticles) {
		// Go through particles.
		for(int i = 0; i < TOTAL_PARTICLES; ++i) {

			if(particles[i] != NULL) {
				if(particles[i]->isDead()) {
					delete particles[i];
					//if(camera.x > 0 && camera.x < LEVEL_WIDTH - camera.w) {
					//std::cout << "hit " << mPosX << std::endl;
					//particles[i] = new Particle((SCREEN_WIDTH / 2) - (CHARACTER_WIDTH / 2), mPosY - camera.y);
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

void Character::handleEvent(SDL_Event &e) {
	//If a key was pressed

	if(headJump == true) {
		setVelocityY(0);
		setVelocityY(CHARACTER_VELY);
		headJump = false;
	}
	if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		//Adjust the velocity
		switch(e.key.keysym.sym) {
			case SDLK_SPACE: 
				//if(!isJumping) {
				mVelY = 0; 
				mVelY -= CHARACTER_VELY; 
				//std::cout << mVelY << std::endl;
				//}
				break;
			case SDLK_w: 
				mVelY -= CHARACTER_VELY;
				//std::cout << mVelY << std::endl;
				break;
			case SDLK_s: 
				mVelY += CHARACTER_VELY;
				//std::cout << mVelY << std::endl;
				break;
			//case SDLK_DOWN: mVelY += CHARACTER_VELY; break;
			case SDLK_a: 
				mVelX -= CHARACTER_VELX; 
				break;
			case SDLK_d: 
				mVelX +=  CHARACTER_VELX; 
				break;
			default:
				break;
			case SDLK_f:
				isAttacking = true;
				break;
		}
	}
	//If a key was released
	else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
		//Adjust the velocity
		switch(e.key.keysym.sym) {
			case SDLK_w: mVelY += CHARACTER_VELY; break;
			case SDLK_s: mVelY -= CHARACTER_VELY; break;
			//case SDLK_DOWN: mVelY -= CHARACTER_VELY; break;
			case SDLK_a: mVelX += CHARACTER_VELX; break; 
			case SDLK_d: mVelX -= CHARACTER_VELX; break; 
			default: break;
		}
	}
}

Character::~Character() {
	// Delete particles.
	for(int i = 0; i < TOTAL_PARTICLES; ++i) {
		delete particles[i];
	}
}

void Character::move(Tile *tiles[], std::vector<Npc *> &npcVector, float timeStep) {

	int tileTouched, npcTouched;

	//Move the character left or right
	mPosX += mVelX * timeStep;

	//If the character went too far to the left or right or touched a wall
	if((mPosX < 0) || (mPosX + CHARACTER_WIDTH > LEVEL_WIDTH)) {
		//move back
		if(mPosX < 0) {
			mPosX = 0;
		}
		else {
			mPosX = LEVEL_WIDTH - CHARACTER_WIDTH;
		}
	}
	mBox.x = mPosX;
	tileTouched = touchesWall(mBox, tiles);
	if(tileTouched > -1 && mVelX > 0) {
		mPosX = tiles[tileTouched]->getBox().x - CHARACTER_WIDTH;
	}
	if(tileTouched > -1 && mVelX < 0) {
		mPosX = tiles[tileTouched]->getBox().x + TILE_WIDTH;
	}
	npcTouched = touchesNpc(mBox, npcVector);
	if(npcTouched > -1 && mVelX > 0) {
		mPosX = npcVector[npcTouched]->getPosX() - CHARACTER_WIDTH;
		if(isAttacking) {
			swordReach = true;
		}
	}
	if(npcTouched > -1 && mVelX < 0) {
		mPosX = npcVector[npcTouched]->getPosX() + npcVector[npcTouched]->NPC_WIDTH;
		if(isAttacking) {
			swordReach = true;
		}
	}
	mBox.x = mPosX;

	//Move the character up or down
	mPosY += mVelY * timeStep;

	//If the character went too far up or down or touched a wall
	if((mPosY < 0) || (mPosY + CHARACTER_HEIGHT > LEVEL_HEIGHT)) {
		if(mPosY < 0) mPosY = 0;
		else {
			mPosY = LEVEL_HEIGHT - CHARACTER_HEIGHT;
			isJumping = false;
		}
	} 
	mBox.y = mPosY;
	tileTouched = touchesWall(mBox, tiles);
	if(tileTouched > -1 && mVelY > 0) {
		mPosY = tiles[tileTouched]->getBox().y - CHARACTER_HEIGHT;
		isJumping = false;
	}
	if(tileTouched > -1 && mVelY < 0) {
		mPosY = tiles[tileTouched]->getBox().y + TILE_HEIGHT;
	}
	npcTouched = touchesNpc(mBox, npcVector);
	if(npcTouched > -1 && mVelY > 0) {
		mPosY = npcVector[npcTouched]->getPosY() - CHARACTER_HEIGHT;
		headJump = true;
		//mVelY = 0;.
		isJumping = false;
		npcCollided = npcTouched;
		//delete npcVector[npcTouched];
		//npcVector.erase(npcVector.begin() + npcTouched);
		//delete npcVector[npcTouched];
		//npcVector[npcTouched] = NULL;
	}
	if(npcTouched > -1 && mVelY < 0) {
		mPosY = npcVector[npcTouched]->getPosY() + npcVector[npcTouched]->NPC_HEIGHT;
	}
	mBox.y = mPosY;
}

void Character::setCamera(SDL_Rect &camera) {
	//Center the camera over the character
	camera.x = (mPosX + CHARACTER_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera.y = (mPosY + CHARACTER_HEIGHT / 2) - SCREEN_HEIGHT / 2;

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

void Character::render(SDL_Rect &camera, bool toggleParticles, int &frame) {
	//Show the character
	if(isAttacking) {
		if(currentFrame / ATTACKING_FRAMES < ATTACKING_FRAMES) {
			int indexAttack = ANIMATION_FRAMES + (currentFrame / ATTACKING_FRAMES);
			currentClip = &spriteClips[indexAttack];
			++currentFrame;
		}
		else {
			currentFrame = 0;
			isAttacking = false;
			currentClip = &spriteClips[0];
		}
	}
	else {
		if(getVelocityX() > 0) {
			currentClip = &spriteClips[frame / ANIMATION_FRAMES];
			flip = SDL_FLIP_HORIZONTAL;
		}
		else if(getVelocityX() < 0) {
			currentClip = &spriteClips[frame / ANIMATION_FRAMES];
			flip = SDL_FLIP_NONE;
		}
		else currentClip = &spriteClips[1];
	}
	// XXX BTON - YOU LEFT OFF HERE
	//currentClip = &spriteClips[0];
	// XXX BTON - CHANGE SOMETHING HERE.

	if(flip == SDL_FLIP_HORIZONTAL) {
		// To adjust for clipping size.
		characterTexture.render((int)(mPosX) - camera.x - currentClip->w + CHARACTER_WIDTH, (int)(mPosY) - camera.y - currentClip->h + CHARACTER_HEIGHT, currentClip, 0, NULL, flip);
	}
	else {
		characterTexture.render((int)(mPosX) - camera.x, (int)(mPosY) - camera.y - currentClip->h + CHARACTER_HEIGHT, currentClip, 0, NULL, flip);
	}

	// Show particles on top of character.
	renderParticles(camera, toggleParticles);
}
