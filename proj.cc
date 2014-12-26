#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "globals.hpp"
#include "texture.hpp"
#include "tiles.hpp"
#include "particle.hpp"
#include "timer.hpp"
#include "npc.hpp"

//The window we'll be rendering to
SDL_Window *gWindow;

//The window renderer
SDL_Renderer *gRenderer; 

// Globally used font.
TTF_Font *gFont = NULL;
LTexture gTextCoordinates;
LTexture gFpsTextTexture;
LTexture gTextVelocity;

//Scene textures
LTexture gTileTexture;
SDL_Rect gTileClips[TOTAL_TILE_SPRITES];

LTexture gBGTexture;

LTexture gRedTexture;
LTexture gGreenTexture;
LTexture gBlueTexture;
LTexture gShimmerTexture;

//The dot that will move around on the screen
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
		int DOT_VELY = 15 * SCREEN_FPS; // 15;
		int DOT_VELX = 10 * SCREEN_FPS; //10;

		//Initializes the variables allocates particles.
		Dot();

		// Deallocates particles.
		~Dot();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent(SDL_Event &e);

		//Moves the dot and check collision against tiles
		void move(Tile *tiles[], Npc &npc, float timeStep);

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

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia(Tile *tiles[]);

//Frees media and shuts down SDL
void close(Tile *tiles[]);

//Sets tiles from tile map
bool setTiles(Tile *tiles[], std::string mapName);

Dot::Dot() {
	//Initialize the collision box
	mPosX = DOT_WIDTH + TILE_WIDTH;
	mPosY = 0;

	mBox.x = 0;
	mBox.y = 0;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;
	isJumping = false;
	isMoving = false;
	flip = SDL_FLIP_NONE;

	//Load dot texture
	if(!dotTexture.loadFromFile("character.png")) {
		printf("Failed to load dot texture!\n");
	}
	else {
		int x = 0; 
		int y = 0;
		for(int i = 0; i < ANIMATION_FRAMES; ++i) {
			spriteClips[i].x = x;
			spriteClips[i].y = y;
			spriteClips[i].w = DOT_WIDTH;
			spriteClips[i].h = DOT_HEIGHT;

			x += DOT_WIDTH + 1;
			if(x >=  SPRITESHEET_WIDTH) {
				x = 0;
				y += TILE_HEIGHT;
			}
		}
	}

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	// Initialize the particles.
	for(int i = 0; i < TOTAL_PARTICLES; ++i) {
		particles[i] = new Particle(mPosX, mPosY, mBox);
	}

}

Dot::~Dot() {
	// Delete particles.
	for(int i = 0; i < TOTAL_PARTICLES; ++i) {
		delete particles[i];
	}
}

void Dot::renderParticles(SDL_Rect &camera, bool toggleParticles) {
	if(toggleParticles) {
		// Go through particles.
		for(int i = 0; i < TOTAL_PARTICLES; ++i) {

			if(particles[i] != NULL) {
				if(particles[i]->isDead()) {
					delete particles[i];
					//if(camera.x > 0 && camera.x < LEVEL_WIDTH - camera.w) {
					//std::cout << "hit " << mPosX << std::endl;
					//particles[i] = new Particle((SCREEN_WIDTH / 2) - (DOT_WIDTH / 2), mPosY - camera.y);
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

void Dot::handleEvent(SDL_Event &e) {
	//If a key was pressed
	if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		//Adjust the velocity
		switch(e.key.keysym.sym) {
			case SDLK_SPACE: 
				if(!isJumping) {
					isJumping = true;
					mVelY = 0; 
					mVelY -= DOT_VELY; 
				}
				break;
			//case SDLK_DOWN: mVelY += DOT_VELY; break;
			case SDLK_LEFT: 
				isMoving = true;
				flip = SDL_FLIP_NONE;
				mVelX -= DOT_VELX; 
				break;
			case SDLK_RIGHT: 
				isMoving = true;
				flip = SDL_FLIP_HORIZONTAL;
				mVelX += DOT_VELX; 
				break;
		}
	}
	//If a key was released
	else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
		//Adjust the velocity
		switch(e.key.keysym.sym) {
			//case SDLK_SPACE: mVelY += DOT_VELY; break;
			//case SDLK_DOWN: mVelY -= DOT_VELY; break;
			case SDLK_LEFT: isMoving = false; mVelX += DOT_VELX; break;
			case SDLK_RIGHT: isMoving = false; mVelX -= DOT_VELX; break;
		}
	}
}

//Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

//Checks collision box against set of tiles
int touchesWall(SDL_Rect box, Tile *tiles[]);

int touchesNpc(SDL_Rect box, SDL_Rect npcBox);

void Dot::move(Tile *tiles[], Npc &npc, float timeStep) {

	int tileTouched, npcTouched;

	//Move the dot left or right
	mPosX += mVelX * timeStep;

	//If the dot went too far to the left or right or touched a wall
	if((mPosX < 0) || (mPosX + DOT_WIDTH > LEVEL_WIDTH)) {
		//move back
		if(mPosX < 0) {
			mPosX = 0;
		}
		else {
			mPosX = LEVEL_WIDTH - DOT_WIDTH;
		}
	}
	mBox.x = mPosX;
	tileTouched = touchesWall(mBox, tiles);
	if(tileTouched > -1 && mVelX > 0) {
		mPosX = tiles[tileTouched]->getBox().x - DOT_WIDTH;
	}
	if(tileTouched > -1 && mVelX < 0) {
		mPosX = tiles[tileTouched]->getBox().x + TILE_WIDTH;
	}
	npcTouched = touchesNpc(mBox, npc.getBoxPosition());
	if(npcTouched == 0 && mVelX > 0) {
		mPosX = npc.getPosX() - DOT_WIDTH;
	}
	if(npcTouched == 0 && mVelX < 0) {
		mPosX = npc.getPosX() + npc.NPC_WIDTH;
	}
	mBox.x = mPosX;

	//Move the dot up or down
	mPosY += mVelY * timeStep;

	//If the dot went too far up or down or touched a wall
	if((mPosY < 0) || (mPosY + DOT_HEIGHT > LEVEL_HEIGHT)) {
		if(mPosY < 0) mPosY = 0;
		else {
			mPosY = LEVEL_HEIGHT - DOT_HEIGHT;
			isJumping = false;
		}
	} 
	mBox.y = mPosY;
	tileTouched = touchesWall(mBox, tiles);
	if(tileTouched > -1 && mVelY > 0) {
		mPosY = tiles[tileTouched]->getBox().y - DOT_HEIGHT;
		isJumping = false;
	}
	if(tileTouched > -1 && mVelY < 0) {
		mPosY = tiles[tileTouched]->getBox().y + TILE_HEIGHT;
	}
	npcTouched = touchesNpc(mBox, npc.getBoxPosition());
	if(npcTouched == 0 && mVelY > 0) {
		mPosY = npc.getPosY() - DOT_HEIGHT;
		isJumping = false;
	}
	if(npcTouched == 0 && mVelY < 0) {
		mPosY = npc.getPosY() + npc.NPC_HEIGHT;
	}
	mBox.y = mPosY;
}

void Dot::setCamera(SDL_Rect &camera) {
	//Center the camera over the dot
	camera.x = (mPosX + DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera.y = (mPosY + DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;

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

void Dot::render(SDL_Rect &camera, bool toggleParticles, SDL_Rect *clip) {
	//Show the dot
	dotTexture.render(mPosX - camera.x, mPosY - camera.y, clip, 0, NULL, flip);

	// Show particles on top of dot.
	renderParticles(camera, toggleParticles);
}

bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Set texture filtering to linear
		if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(gWindow == NULL) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if(gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if(!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				// Initialize SDL_ttf.
				if(TTF_Init() == -1 ) {
					printf("SDL_ttf failed to initialize, error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia(Tile *tiles[]) {
	//Loading success flag
	bool success = true;

	// Load red texture.
	if(!gRedTexture.loadFromFile("red.bmp")) {
		printf("failed to load red texture\n");
		success = false;
	}

	// Load green texture.
	if(!gGreenTexture.loadFromFile("green.bmp")) {
		printf("failed to load green texture\n");
		success = false;
	}

	// Load blue texture.
	if(!gBlueTexture.loadFromFile("blue.bmp")) {
		printf("failed to load blue texture\n");
		success = false;
	}

	// Load shimmer texture.
	if(!gShimmerTexture.loadFromFile("shimmer.bmp")) {
		printf("failed to load shimmer texture\n");
		success = false;
	}

	// Load background texture.
	if(!gBGTexture.loadFromFile("gamebackground.png")) {
		printf( "Failed to load background texture!\n" );
		success = false;
	}

	// Set texture transparency.
	gRedTexture.setAlpha(172);
	gGreenTexture.setAlpha(172);
	gBlueTexture.setAlpha(172);
	gShimmerTexture.setAlpha(172);

	//Load tile texture
	if(!gTileTexture.loadFromFile("tiles.png")) {
		printf("Failed to load tile set texture!\n");
		success = false;
	}

	//Load tile map
	if(!setTiles(tiles, "lazy.map")) {
		printf("Failed to load tile set!\n");
		success = false;
	}

	// Opening the font.
	gFont = TTF_OpenFont("lazy.ttf", 28); 
	if(gFont == NULL) {
		printf("failed to load font, error: %s\n", TTF_GetError());
		success = false;
	}

	return success;
}

void close(Tile *tiles[]) {
	//Deallocate tiles
	for(int i = 0; i < TOTAL_TILES; ++i) {
		if(tiles[i] == NULL) {
			delete tiles[i];
			tiles[i] = NULL;
		}
	}

	//Free loaded images
	gRedTexture.free();
	gBlueTexture.free();
	gGreenTexture.free();
	gShimmerTexture.free();
	gTileTexture.free();
	gTextCoordinates.free();
	gTextVelocity.free();
	gBGTexture.free();
	gFpsTextTexture.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	gFont = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision(SDL_Rect a, SDL_Rect b) {
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if(bottomA <= topB) {
		return false;
	}

	if(topA >= bottomB) {
		return false;
	}

	if(rightA <= leftB) {
		return false;
	}

	if(leftA >= rightB) {
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

bool setTiles(Tile *tiles[], std::string mapName) {
	//Success flag
	bool tilesLoaded = true;

	//The tile offsets
	int x = 0, y = 0;

	//Open the map
	std::ifstream map(mapName);

	//If the map couldn't be loaded
	if(map == NULL) {
		printf("Unable to load map file!\n");
		tilesLoaded = false;
	}
	else {
		//Initialize the tiles
		for(int i = 0; i < TOTAL_TILES; ++i) {
			//Determines what kind of tile will be made
			int tileType = -1;

			//Read tile from map file
			map >> tileType;

			//If the was a problem in reading the map
			if(map.fail()) {
				//Stop loading map
				printf("Error loading map: Unexpected end of file!\n");
				tilesLoaded = false;
				break;
			}

			//If the number is a valid tile number
			if((tileType >= 0) && (tileType < TOTAL_TILE_SPRITES)) {
				tiles[i] = new Tile(x, y, tileType);
			}
			//If we don't recognize the tile type
			else {
				//Stop loading map
				printf("Error loading map: Invalid tile type at %d!\n", i);
				tilesLoaded = false;
				break;
			}

			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if(x >= LEVEL_WIDTH) {
				//Move back
				x = 0;

				//Move to the next row
				y += TILE_HEIGHT;
			}
		}

		//Clip the sprite sheet
		if(tilesLoaded) {
			x = 0; y =0;
			for(int i = 0; i < TOTAL_TILE_SPRITES; ++i) {
				gTileClips[i].x = x;
				gTileClips[i].y = y;
				gTileClips[i].w = TILE_WIDTH;
				gTileClips[i].h = TILE_HEIGHT;

				x += TILE_WIDTH;
				if(x >=  TILESHEET_WIDTH) {
					x = 0;
					y += TILE_HEIGHT;
				}
			}

		}
	}

	//Close the file
	map.close();

	//If the map was loaded fine
	return tilesLoaded;
}

int touchesWall(SDL_Rect box, Tile *tiles[]) {
	//Go through the tiles
	for(int i = 0; i < TOTAL_TILES; ++i) {
		//If the tile is a wall type tile
		if(tiles[i]->getType() % 4 != 0 && tiles[i]->getType() < 20) {
			//If the collision box touches the wall tile
			if(checkCollision(box, tiles[i]->getBox())) {
				return i;
			}
		}
	}

	//If no wall tiles were touched
	return -1;
}

int touchesNpc(SDL_Rect box, SDL_Rect npcBox) {
	//Go through the npc
	if(checkCollision(box, npcBox)) {
		return 0;
	}

	//If no wall tiles were touched
	return -1;
}

int main(int argc, char *args[]) {
	
restart:
	bool restart = false;
	//Start up SDL and create window
	if(!init()) {
		printf("Failed to initialize!\n");
	}
	else {
		//The level tiles
		Tile *tileSet[TOTAL_TILES];

		//Load media
		if(!loadMedia(tileSet)) {
			printf("Failed to load media!\n");
		}

		else {
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
			Dot dot;
			Npc *npcContainer[TOTAL_NPCS];
			npcContainer[0] = new Npc(LEVEL_WIDTH / 2, 0, "character2.png");
			npcContainer[1] = new Npc(LEVEL_WIDTH / 2 - 100, 0, "character3.png");
			Npc npc(LEVEL_WIDTH / 2, 0, "character2.png");
			Npc npc2(LEVEL_WIDTH / 2 - 100, 0, "character3.png");

			// Timer.
			LTimer stepTimer;

			//Level camera
			SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

			float acceleration = 1.07 * SCREEN_FPS;
			bool toggleParticles = true;
			SDL_Color textColor = {136, 0, 21};
			std::ostringstream os;

			std::stringstream timeText;
			// Frames per second timer.
			LTimer fpsTimer;

			// Frames per second cap timer.
			LTimer capTimer;

			// Npc timer.
			LTimer npcTimer;

			// Start timer.
			int countedFrames = 0;
			fpsTimer.start();

			// Current animation frame.
			int frame = 0;
			SDL_Rect *currentClip;

			// Background scrolling offset.
			int scrollingOffset = 0;

			npcTimer.start();

			//While application is running
			while(!quit && !restart) {

				Uint32 ticks = SDL_GetTicks();

				// Start cap timer.
				capTimer.start();


				//Handle events on queue

				while(SDL_PollEvent(&e) != 0) {

					//User requests quit
					if(e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE) {
						quit = true;
					}
					if(e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_r) {
						restart = true;
					}
					if(e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_1) {
						setTiles(tileSet, "lazy2.map");
					}
					if(e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_2) {
						setTiles(tileSet, "lazy.map");
					}
					if(e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_4) {
						setTiles(tileSet, "lazy3.map");
					}
					if(e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_3) {
						toggleParticles = !toggleParticles;
					}

					// input for the dot
					dot.handleEvent(e);
				}

				Uint32 seconds = ticks / 1000.f;

				// Calculate and correct fps.
				float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
				if(avgFPS > 2000000) avgFPS = 0;


				// Calculate time step.
				float timeStep = stepTimer.getTicks() / 1000.f; //1.0;

				// FPS text.
				timeText.str("");
				timeText << "FPS: " << avgFPS;
				if(!gFpsTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor)) {
					printf("unable to render FPS texture\n");
				}

				// Gravity implementation.
				if(dot.getVelocityY() < 15 * SCREEN_FPS) {
					dot.setVelocityY(dot.getVelocityY() + acceleration);
				}
				if(dot.getVelocityY() > 15 * SCREEN_FPS) {
					dot.setVelocityY(15 * avgFPS);
				}
				if(dot.getVelocityY() < -20 * SCREEN_FPS) {
					dot.setVelocityY(-20 * avgFPS);
				}

				if(npc.getVelocityY() < 15 * SCREEN_FPS) {
					npc.setVelocityY(npc.getVelocityY() + acceleration);
				}
				if(npc.getVelocityY() > 15 * SCREEN_FPS) {
					npc.setVelocityY(15 * avgFPS);
				}
				if(npc.getVelocityY() < -20 * SCREEN_FPS) {
					npc.setVelocityY(-20 * avgFPS);
				}

				if(npc2.getVelocityY() < 15 * SCREEN_FPS) {
					npc2.setVelocityY(npc2.getVelocityY() + acceleration);
				}
				if(npc2.getVelocityY() > 15 * SCREEN_FPS) {
					npc2.setVelocityY(15 * avgFPS);
				}
				if(npc2.getVelocityY() < -20 * SCREEN_FPS) {
					npc2.setVelocityY(-20 * avgFPS);
				}

				if((npcTimer.getTicks() / 1000) != 0 && (npcTimer.getTicks() / 1000) % 2  == 0) {
					//std::cout << "hit " << npc.getVelocityX() << std::endl;
					switch(rand() % 3) {
						case 0:
							npc2.isMoving = true;
							npc2.setVelocityX(-npc2.NPC_VELX);
							npc2.flip = SDL_FLIP_NONE;
							break;
						case 1:
							npc2.isMoving = true;
							npc2.setVelocityX(npc2.NPC_VELX);
							npc2.flip = SDL_FLIP_HORIZONTAL;
							break;
						case 2:
							npc2.isMoving = false;
							npc2.setVelocityX(0);
							break;
					}
					switch(rand() % 3) {
						case 0:
							npc.isMoving = true;
							npc.setVelocityX(-npc.NPC_VELX);
							npc.flip = SDL_FLIP_NONE;
							break;
						case 1:
							npc.isMoving = true;
							npc.setVelocityX(npc.NPC_VELX);
							npc.flip = SDL_FLIP_HORIZONTAL;
							break;
						case 2:
							npc.isMoving = false;
							npc.setVelocityX(0);
							break;
					}
					npcTimer.start();
					
				}

				//Move the dot.
				dot.move(tileSet, npc, timeStep);
				npc.move(tileSet, timeStep);
				npc2.move(tileSet, timeStep);
				dot.setCamera(camera);

				// Scroll background.
				--scrollingOffset;
				if(scrollingOffset < -gBGTexture.getWidth()) {
					scrollingOffset = 0;
				}

				// Restart step timer.
				stepTimer.start();
				
				os.str("");
				os << dot.getBoxPosition().x << ", " << dot.getBoxPosition().y;
				if(!gTextCoordinates.loadFromRenderedText(os.str(), textColor)) {
					printf("failed to render text texture\n");
					quit = true;
				}

				os.str("");
				os << dot.getVelocityX() << ", " << dot.getVelocityY();
				if(!gTextVelocity.loadFromRenderedText(os.str(), textColor)) {
					printf("failed to render text texture\n");
					quit = true;
				}

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				gBGTexture.render(scrollingOffset, 0);
				gBGTexture.render(scrollingOffset + gBGTexture.getWidth(), 0);

				//Render level
				for(int i = 0; i < TOTAL_TILES; ++i) {
					tileSet[i]->render(camera);
				}

				// Render font.
				gTextCoordinates.render((SCREEN_WIDTH - gTextCoordinates.getWidth()),  0);
				gTextVelocity.render((SCREEN_WIDTH - gTextVelocity.getWidth()),  30);
				gFpsTextTexture.render((SCREEN_WIDTH - gFpsTextTexture.getWidth()),  60);

				//Render dot
				//Uint32 sprite = seconds % 4;

				if(dot.isMoving) 
					currentClip = &dot.spriteClips[frame / dot.ANIMATION_FRAMES];
				else currentClip = &dot.spriteClips[1];
				dot.render(camera, toggleParticles, currentClip);

				if(npc.isMoving) currentClip = &npc.spriteClips[frame / npc.ANIMATION_FRAMES];
				else currentClip = &npc.spriteClips[1]; 
				npc.render(camera, toggleParticles, currentClip);
				
				if(npc2.isMoving) currentClip = &npc2.spriteClips[frame / npc2.ANIMATION_FRAMES];
				else currentClip = &npc2.spriteClips[1]; 
				npc2.render(camera, toggleParticles, currentClip);
				
				// Next frame.
				++frame;
				// Cycle.
				if(frame / dot.ANIMATION_FRAMES >= dot.ANIMATION_FRAMES) frame = 0;

				//Update screen
				SDL_RenderPresent(gRenderer);
				++countedFrames;

				// If frame finished early.
				int frameTicks = capTimer.getTicks();
				if(frameTicks < SCREEN_TICKS_PER_FRAME) {
					// Wait remaining time.
					SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
				}
			}
			dot.dotTexture.free();
			npc.npcTexture.free();
			npc2.npcTexture.free();
		}

		//Free resources and close SDL
		close(tileSet);
		if(restart) goto restart;
	}

	return 0;
}
