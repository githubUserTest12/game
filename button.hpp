#ifndef BUTTON_HPP
	#define BUTTON_HPP
#include <SDL.h>
#include "globals.hpp"

enum LButtonSprite {
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
	BUTTON_SPRITE_MOUSE_DOWN = 2,
	BUTTON_SPRITE_MOUSE_UP = 3,
	BUTTON_SPRITE_TOTAL = 4
};

class LButton {
	public:
		LButton();

		// Sets top left position.
		void setPosition(int x, int y);

		// Handles mouse event.
		void handleEvent(SDL_Event *e);

		// Shows button sprite.
		void render();

		SDL_Rect dstrect = { SCREEN_WIDTH - 400, 5, 200, 100 };

	private:

		// Top left position.
		SDL_Point mPosition;

		// Currently used global sprite.
		LButtonSprite mCurrentSprite;
};
#endif
