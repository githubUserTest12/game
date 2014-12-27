#ifndef NPC_HPP
	#define NPC_HPP
#include <SDL.h>
#include "tiles.hpp"
#include "globals.hpp"

extern int touchesWall(SDL_Rect box, Tile *tiles[]);

class Npc {
	public:
		//The dimensions of the dot
		static const int NPC_WIDTH = 38;
		static const int NPC_HEIGHT = 55;
		static const int ANIMATION_FRAMES = 4;
		static const int SPRITESHEET_WIDTH = ANIMATION_FRAMES * NPC_WIDTH;

		//Maximum axis velocity of the dot
		int NPC_VELY = 15;
		int NPC_VELX = 1;

		// Texture.
		LTexture npcTexture;
		SDL_Rect spriteClips[ANIMATION_FRAMES];

		//Initializes the variables, allocates particles.
		Npc(int x, int y, std::string filename);

		// Deallocates particles.
		~Npc();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent(SDL_Event &e);

		//Moves the dot and check collision against tiles
		void move(Tile *tiles[], float timeStep);

		//Centers the camera over the dot
		//void setCamera(SDL_Rect &camera);

		//Shows the dot on the screen
		void render(SDL_Rect &camera, bool toggleParticles = false, SDL_Rect *clip = NULL);

		bool isJumping;
		bool isMoving;
		bool isDead;
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
		//Particle *particles[TOTAL_PARTICLES];

		// Render particles.
		//void renderParticles(SDL_Rect &camera, bool toggleParticles);

		//Collision box of the dot
		SDL_Rect mBox;
		float mPosX, mPosY;

		//The velocity of the dot
		float mVelX, mVelY;
};
#endif
