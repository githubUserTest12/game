#ifndef CHARACTER_HPP
	#define CHARACTER_HPP
#include "globals.hpp"
#include "tiles.hpp"
#include "npc.hpp"
#include "particle.hpp"

class Dot {
	public:
		//The dimensions of the dot

		static const int DOT_WIDTH = 38;
		static const int DOT_HEIGHT = 55;
		static const int ANIMATION_FRAMES = 4;
		static const int SPRITESHEET_WIDTH = ANIMATION_FRAMES * DOT_WIDTH;
		//static const int SPRITESHEET_HEIGHT = 40;

		LTexture dotTexture;
		SDL_Rect spriteClips[ANIMATION_FRAMES];

		//Maximum axis velocity of the dot
		const int DOT_VELY = 15; // * SCREEN_FPS; // 15;
		const int DOT_VELX = 10; // * SCREEN_FPS; //10;
		const int GRAVITY_CONSTANT = 60;

		//Initializes the variables allocates particles.
		Dot();

		// Deallocates particles.
		~Dot();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent(SDL_Event &e);

		//Moves the dot and check collision against tiles
		void move(Tile *tiles[], Npc *npc[], float timeStep);

		//Centers the camera over the dot
		void setCamera(SDL_Rect &camera);

		//Shows the dot on the screen
		void render(SDL_Rect &camera, bool toggleParticles, SDL_Rect *clip);

		bool isJumping;
		bool isMoving;
		SDL_RendererFlip flip;

		inline SDL_Rect getBoxPosition() {
			return mBox;
		}

		inline float getPosX() {
			return mPosX;
		}

		inline float getPosY() {
			return mPosY;
		}

		inline float getVelocityX() {
			return mVelX;
		}

		inline float getVelocityY() {
			return mVelY;
		}

		inline void setVelocityX(float velocity) {
			mVelX = velocity;
		}

		inline void setVelocityY(float velocity) {
			mVelY = velocity;
		}


	private:

		// Particles.
		Particle *particles[TOTAL_PARTICLES];

		// Render particles.
		void renderParticles(SDL_Rect &camera, bool toggleParticles);

		//Collision box of the dot
		SDL_Rect mBox;
		float mPosX, mPosY;

		//The velocity of the dot
		float mVelX, mVelY;
};

#endif
