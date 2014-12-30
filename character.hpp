#ifndef CHARACTER_HPP
	#define CHARACTER_HPP
#include <vector>
#include "globals.hpp"
#include "tiles.hpp"
#include "npc.hpp"
#include "particle.hpp"

class Character {
	public:
		//The dimensions of the character

		static const int CHARACTER_WIDTH = 38;
		static const int CHARACTER_HEIGHT = 55;
		static const int ANIMATION_FRAMES = 4;
		static const int ATTACKING_FRAMES = 8;
		static const int TOTAL_FRAMES = ANIMATION_FRAMES + ATTACKING_FRAMES;
		static const int SPRITESHEET_WIDTH = ANIMATION_FRAMES * CHARACTER_WIDTH;
		//static const int SPRITESHEET_HEIGHT = 40;

		LTexture characterTexture;
		SDL_Rect spriteClips[TOTAL_FRAMES];

		//Maximum axis velocity of the character
		const int CHARACTER_VELY = 15; // * SCREEN_FPS; // 15;
		const int CHARACTER_VELX = 10; // * SCREEN_FPS; //10;
		const int GRAVITY_CONSTANT = 60;

		bool headJump;
		bool swordReach;
		bool isAttacking;
		int npcCollided;
		SDL_Rect *currentClip = NULL;
		int currentFrame;

		//Initializes the variables allocates particles.
		Character();

		// Deallocates particles.
		~Character();

		//Takes key presses and adjusts the character's velocity
		void handleEvent(SDL_Event &e);

		//Moves the character and check collision against tiles
		void move(Tile *tiles[], std::vector<Npc *> &npcVector, float timeStep);

		//Centers the camera over the character
		void setCamera(SDL_Rect &camera);

		//Shows the character on the screen
		void render(SDL_Rect &camera, bool toggleParticles, int &frame);

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

		//Collision box of the character
		SDL_Rect mBox;
		float mPosX, mPosY;

		//The velocity of the character
		float mVelX, mVelY;
};

#endif
