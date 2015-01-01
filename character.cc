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
	mWeapon = mBox;
	isJumping = false;
	isMoving = false;
	isAttacking = false;
	headJump = false;
	//npcStabbed = 0;
	flip = SDL_FLIP_NONE;
	attackingFrame = 0;
	walkingFrame = 0;
	frameRate = 40;

	//Load character texture
	if(!characterTexture.loadFromFile("zerowalk.png")) {
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
		/*
		if(ATTACKING_FRAMES > 0) {
			x = 0;
			y = 0;
			for(int i =	0; i < ATTACKING_FRAMES; ++i) {
				spriteClips[i].x = x;
				spriteClips[i].y = y;
				spriteClips[i].w = 50;
				spriteClips[i].h = 50;

				x += CHARACTER_WIDTH;
				if(x >= SPRITESHEET_WIDTH) {
					x = 0;
					y += CHARACTER_HEIGHT;
				}
			}
		}
		*/
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

	/*
	if(headJump == true) {
		setVelocityY(0);
		setVelocityY(CHARACTER_VELY);
		headJump = false;
	}
	*/
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
	mWeapon.x = mPosX;
	tileTouched = touchesWall(mBox, tiles);
	if(tileTouched > -1 && mVelX > 0) {
		mPosX = tiles[tileTouched]->getBox().x - CHARACTER_WIDTH;
	}
	if(tileTouched > -1 && mVelX < 0) {
		mPosX = tiles[tileTouched]->getBox().x + TILE_WIDTH;
	}

	if(isAttacking && flip == SDL_FLIP_NONE) mWeapon.x = mPosX + 50;
	else if(isAttacking && flip == SDL_FLIP_HORIZONTAL) mWeapon.x = mPosX - 50;
	npcTouched = touchesNpc(mWeapon, npcVector);
	// Handle weapon.
	if(npcTouched > -1 && isAttacking && flip == SDL_FLIP_NONE) {
		npcVector[npcTouched]->wasStabbed = true;
		npcVector[npcTouched]->setVelocityX(15 * 60);
		npcVector[npcTouched]->wasAttackedTimer.start();
	}
	else if(npcTouched > -1 && isAttacking && flip == SDL_FLIP_HORIZONTAL) {
		npcVector[npcTouched]->wasStabbed = true;
		npcVector[npcTouched]->setVelocityX(-15 * 60);
		npcVector[npcTouched]->wasAttackedTimer.start();
	}

	// Handle character.
	npcTouched = touchesNpc(mBox, npcVector);
	if(npcTouched > -1 && mVelX > 0) {
		mPosX = npcVector[npcTouched]->getPosX() - CHARACTER_WIDTH;
		/*
		if(isAttacking) {
			npcVector[npcTouched]->wasStabbed = true;
			npcVector[npcTouched]->setVelocityX(15 * 60);
			npcVector[npcTouched]->wasAttackedTimer.start();
		}
		*/
	}
	if(npcTouched > -1 && mVelX < 0) {
		mPosX = npcVector[npcTouched]->getPosX() + npcVector[npcTouched]->NPC_WIDTH;
		/*
		if(isAttacking) {
			npcVector[npcTouched]->wasStabbed = true;
			npcVector[npcTouched]->setVelocityX(-15 * 60);
			npcVector[npcTouched]->wasAttackedTimer.start();
		}
		*/
	}
	mBox.x = mPosX;

	// Gravity.
	if(mVelY < 900) {
		mVelY += 3600 * timeStep;
	}
	else if(mVelY > 900) {
		mVelY = 900;
	}
	//Move the character up or down
	mPosY += mVelY * timeStep;

	//If the character went too far up or down or touched a wall
	if((mPosY < 0) || (mPosY + CHARACTER_HEIGHT > LEVEL_HEIGHT)) {
		if(mPosY < 0) mPosY = 0;
		else {
			mVelY = 0;
			mPosY = LEVEL_HEIGHT - CHARACTER_HEIGHT;
			isJumping = false;
		}
	} 
	mBox.y = mPosY;
	mWeapon.y = mPosY;
	tileTouched = touchesWall(mBox, tiles);
	if(tileTouched > -1 && mVelY > 0) {
		mVelY = 0;
		mPosY = tiles[tileTouched]->getBox().y - CHARACTER_HEIGHT;
		isJumping = false;
	}
	else if(mPosY + CHARACTER_HEIGHT > LEVEL_HEIGHT) {
	}
	if(tileTouched > -1 && mVelY < 0) {
		mPosY = tiles[tileTouched]->getBox().y + TILE_HEIGHT;
	}
	npcTouched = touchesNpc(mBox, npcVector);
	if(npcTouched > -1 && mVelY > 0) {
		if(!isAttacking) {
			mPosY = npcVector[npcTouched]->getPosY() - CHARACTER_HEIGHT;
		}
		// Hack for now, don't want to decrease frame rate cap.
		if(mVelY > 60) headJump = true;
		isJumping = false;
		npcVector[npcTouched]->wasJumped = true;
	}
	if(npcTouched > -1 && mVelY < 0) {
		if(!isAttacking) mPosY = npcVector[npcTouched]->getPosY() + npcVector[npcTouched]->NPC_HEIGHT;
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

void Character::render(SDL_Rect &camera, bool toggleParticles) {
	//Show the character
	if(isAttacking) {
		if(!attackingTimer.isStarted()) attackingTimer.start();
		currentClip = &spriteClips[attackingFrame];
		attackingFrame = (attackingTimer.getTicks() / frameRate) % ANIMATION_FRAMES;
		if(attackingFrame > 0) firstAttack = true;
		if(attackingFrame % ANIMATION_FRAMES == 0 && firstAttack == true) {
			firstAttack = false;
			attackingTimer.stop();
			attackingFrame = 0;
			isAttacking = false;
			currentClip = &spriteClips[0];
		}
	}
	else {
		// Really complicated implementation...

		if(getVelocityX() > 0) {
			if(!walkingTimer.isStarted()) walkingTimer.start();
			currentClip = &spriteClips[walkingFrame];
			if(firstWalk == true) {
				walkingFrame = ((walkingTimer.getTicks() / frameRate) + 2) % ANIMATION_FRAMES;
			}
			else walkingFrame = (walkingTimer.getTicks() / frameRate) % ANIMATION_FRAMES;
			if(walkingFrame > 0) firstWalk = true;
			if(walkingFrame % ANIMATION_FRAMES == 0 && firstWalk == true) {
				walkingTimer.start();
				walkingFrame = ((walkingTimer.getTicks() / frameRate) + 2) % ANIMATION_FRAMES;
			}
			flip = SDL_FLIP_NONE;
		}
		else if(getVelocityX() < 0) {
			if(!walkingTimer.isStarted()) walkingTimer.start();
			currentClip = &spriteClips[walkingFrame];
			if(firstWalk == true) {
				walkingFrame = ((walkingTimer.getTicks() / frameRate) + 2) % ANIMATION_FRAMES;
			}
			else walkingFrame = (walkingTimer.getTicks() / frameRate) % ANIMATION_FRAMES;
			if(walkingFrame > 0) firstWalk = true;
			if(walkingFrame % ANIMATION_FRAMES == 0 && firstWalk == true) {
				walkingTimer.start();
				walkingFrame = ((walkingTimer.getTicks() / frameRate) + 2) % ANIMATION_FRAMES;
			}
			flip = SDL_FLIP_HORIZONTAL;
		}
		else {
			walkingFrame = 0;
			walkingTimer.stop();
			firstWalk = false;
			currentClip = &spriteClips[0];
		}
	}
	// XXX BTON - YOU LEFT OFF HERE
	//currentClip = &spriteClips[0];
	// XXX BTON - CHANGE SOMETHING HERE.

	dstrect.x = (int)(mPosX) - camera.x - dstrect.w + CHARACTER_WIDTH;
	dstrect.y = (int)(mPosY) - camera.y - dstrect.h + CHARACTER_HEIGHT;
	if(flip == SDL_FLIP_NONE) {
		// To adjust for clipping size.
		characterTexture.render((int)(mPosX) - camera.x - currentClip->w + CHARACTER_WIDTH, (int)(mPosY) - camera.y - currentClip->h + CHARACTER_HEIGHT, currentClip, dstrect, 0, NULL, flip);
	}
	else {
		characterTexture.render((int)(mPosX) - camera.x, (int)(mPosY) - camera.y - currentClip->h + CHARACTER_HEIGHT, currentClip, dstrect, 0, NULL, flip);
	}

	// Show particles on top of character.
	renderParticles(camera, toggleParticles);
}
