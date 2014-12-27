#include "particle.hpp"

Particle::Particle(int x, int y, SDL_Rect mBox) {
	// Set offsets.
	mPosX = x - 5 + (rand() % (mBox.w + 5));
	mPosY = y - 5 + (rand() % (mBox.h + 5));

	// Initialize the animation.
	mFrame = rand() % 5;

	// Set type.
	switch(rand() % 3) {
		case 0: mTexture = &gRedTexture; break;
		case 1: mTexture = &gGreenTexture; break;
		case 2: mTexture = &gBlueTexture; break;
	}
}

void Particle::render() {
	// Show image.
	mTexture->render(mPosX, mPosY);

	// show shimmer.
	if(mFrame % 2 == 0) {
		gShimmerTexture.render(mPosX, mPosY);
	}

	// animate.
	mFrame++;
}

bool Particle::isDead() {
	return mFrame > frameCap;
}

