#ifndef NPC_HPP
	#define NPC_HPP
class Npc {
	public:
		//The dimensions of the dot
		static const int NPC_WIDTH = 20;
		static const int NPC_HEIGHT = 20;

		//Maximum axis velocity of the dot
		int NPC_VELY = 15 * SCREEN_FPS; // 15;
		int NPC_VELX = 10 * SCREEN_FPS; //10;

		//Initializes the variables allocates particles.
		Npc();

		// Deallocates particles.
		~Npc();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent(SDL_Event &e);

		//Moves the dot and check collision against tiles
		void move(Tile *tiles[], std::vector<SDL_Rect> &otherColliders, float timeStep);

		// Collision boxes.
		std::vector<SDL_Rect> &getColliders();

		//Centers the camera over the dot
		//void setCamera(SDL_Rect &camera);

		//Shows the dot on the screen
		void render(SDL_Rect &camera, bool toggleParticles = false);

		bool isJumping;
		bool isMoving;

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
