#ifndef NPC_HPP
	#define NPC_HPP
#include <SDL.h>
#include "tiles.hpp"
#include "globals.hpp"
#include "texture.hpp"

extern int touchesWall(SDL_Rect box, Tile *tiles[]);

class Npc {
	public:

		//Maximum axis velocity of the dot
		int NPC_VELY = 15;
		int NPC_VELX = 1;

		//The dimensions of the dot
		const int NPC_WIDTH;
		const int NPC_HEIGHT;
		const int ANIMATION_FRAMES;
		const int SPRITESHEET_WIDTH;

		//Initializes the variables, allocates particles.
		Npc(int x, int y, int width, int height, int maxFrames, std::string filename);

		// Texture.
		LTexture npcTexture;
		std::vector<SDL_Rect> spriteClips;
		SDL_Rect *currentClip;

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
