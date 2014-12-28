#ifndef PARTICLE_HPP
	#define PARTICLE_HPP

#include "globals.hpp"
class Particle {
	public:
		// Initialize position and animation.
		Particle(int x, int y, SDL_Rect mBox);

		~Particle();

		// Shows the particle.
		void render();

		int frameCap = 10;

		// Checks if particle is dead.
		bool isDead();

		inline void setFrame(int value) {
			frameCap = value;
		}

	private:
		// Offsets
		int mPosX, mPosY;
		// Current frame of animation.
		int mFrame;

		// Type of particle.
		LTexture *mTexture;
};
#endif
