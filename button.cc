#include "button.hpp"
#include "globals.hpp"

LButton::LButton() {
	mPosition.x = 0;
	mPosition.y = 0;

	mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y) {
	mPosition.x = x;
	mPosition.y = y;
}

void LButton::handleEvent(SDL_Event *e) {
	// If mouse event happened.
	if(e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
		// Get mouse position.
		int x, y;
		SDL_GetMouseState(&x, &y);

		// Check if mouse is in button.
		bool inside = true;

		// Mouse is left of the button.
		if(x < dstrect.x) {
			inside = false;
		}
		// Right.
		else if(x > dstrect.x + dstrect.w) {
			inside = false;
		}
		else if(y < dstrect.y) {
			inside = false;
		}
		else if(y > dstrect.y + dstrect.h) {
			inside = false;
		}

		if(!inside) {
			mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
		}
		else {
			// Set mouse over sprite.
			switch(e->type) {
				case SDL_MOUSEMOTION:
					mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if(Mix_PlayingMusic() == 0) {
						Mix_PlayMusic(gMusic[rand() % 2], -1);
					}
					else Mix_HaltMusic();
					mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
					break;
				case SDL_MOUSEBUTTONUP:
					mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
					break;
			}
		}
	}
}

void LButton::render() {
	// Show current button sprite.
	gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite], dstrect);
}
