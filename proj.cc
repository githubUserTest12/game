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

class LTimer {
	public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

	private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
};

LTimer::LTimer() {
	//Initialize the variables
	mStartTicks = 0;
	mPausedTicks = 0;

	mPaused = false;
	mStarted = false;
}

void LTimer::start() {
	//Start the timer
	mStarted = true;

	//Unpause the timer
	mPaused = false;

	//Get the current clock time
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop() {
	//Stop the timer
	mStarted = false;

	//Unpause the timer
	mPaused = false;

	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause() {
	//If the timer is running and isn't already paused
	if( mStarted && !mPaused )
	{
		//Pause the timer
		mPaused = true;

		//Calculate the paused ticks
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void LTimer::unpause() {
	//If the timer is running and paused
	if( mStarted && mPaused )
	{
		//Unpause the timer
		mPaused = false;

		//Reset the starting ticks
		mStartTicks = SDL_GetTicks() - mPausedTicks;

		//Reset the paused ticks
		mPausedTicks = 0;
	}
}

Uint32 LTimer::getTicks() {
	//The actual timer time
	Uint32 time = 0;

	//If the timer is running
	if( mStarted )
	{
		//If the timer is paused
		if( mPaused )
		{
			//Return the number of ticks when the timer was paused
			time = mPausedTicks;
		}
		else
		{
			//Return the current time minus the start time
			time = SDL_GetTicks() - mStartTicks;
		}
	}

	return time;
}

bool LTimer::isStarted() {
	//Timer is running and paused or unpaused
	return mStarted;
}

bool LTimer::isPaused() {
	//Timer is running and paused
	return mPaused && mStarted;
}

//The window we'll be rendering to
SDL_Window *gWindow;

//The window renderer
SDL_Renderer *gRenderer; 

// Globally used font.
TTF_Font *gFont = NULL;
LTexture gTextCoordinates;
LTexture gTextVelocity;

//Scene textures
LTexture gDotTexture;
LTexture gTileTexture;
SDL_Rect gTileClips[TOTAL_TILE_SPRITES];


LTexture gRedTexture;
LTexture gGreenTexture;
LTexture gBlueTexture;
LTexture gShimmerTexture;

//The dot that will move around on the screen
class Dot {
	public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VELY = 900; //15;
		static const int DOT_VELX = 600; //10;

		//Initializes the variables allocates particles.
		Dot();

		// Deallocates particles.
		~Dot();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent(SDL_Event &e);

		//Moves the dot and check collision against tiles
		void move(Tile *tiles[], float timeStep);

		//Centers the camera over the dot
		void setCamera(SDL_Rect &camera);

		//Shows the dot on the screen
		void render(SDL_Rect &camera, bool toggleParticles);

		bool isJumping;

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
	mPosX = 0;
	mPosY = 0;

	mBox.x = 0;
	mBox.y = 0;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;
	isJumping = false;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	// Initialize the particles.
	for(int i = 0; i < TOTAL_PARTICLES; ++i) {
		particles[i] = new Particle(getBoxPosition().x, getBoxPosition().y);
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
			// Delete and replace dead particles.
			if(particles[i]->isDead()) {
				delete particles[i];
				particles[i] = new Particle(mPosX - camera.x, mPosY - camera.y);
			}
		}

		// Show particles.
		for(int i = 0; i < TOTAL_PARTICLES; ++i) {
			particles[i]->render();
		}
	}
	else {
		for(int i = 0; i < TOTAL_PARTICLES; ++i) {
			// Delete and replace dead particles.
			delete particles[i];
			particles[i] = new Particle(mPosX - camera.x, mPosY - camera.y);
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
			case SDLK_LEFT: mVelX -= DOT_VELX; break;
			case SDLK_RIGHT: mVelX += DOT_VELX; break;
		}
	}
	//If a key was released
	else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
		//Adjust the velocity
		switch(e.key.keysym.sym) {
			//case SDLK_SPACE: mVelY += DOT_VELY; break;
			//case SDLK_DOWN: mVelY -= DOT_VELY; break;
			case SDLK_LEFT: mVelX += DOT_VELX; break;
			case SDLK_RIGHT: mVelX -= DOT_VELX; break;
		}
	}
}

//Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

//Checks collision box against set of tiles
int touchesWall(SDL_Rect box, Tile *tiles[]);

void Dot::move(Tile *tiles[], float timeStep) {

	int tileTouched;

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
	mBox.y = mPosY;
}

void Dot::setCamera(SDL_Rect &camera) {
	//Center the camera over the dot
	camera.x =(mPosX + DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera.y =(mPosY + DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;

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

void Dot::render(SDL_Rect &camera, bool toggleParticles) {
	//Show the dot
	gDotTexture.render(mPosX - camera.x, mPosY - camera.y);

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

	//Load dot texture
	if(!gDotTexture.loadFromFile("dot.bmp")) {
		printf("Failed to load dot texture!\n");
		success = false;
	}

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
	gDotTexture.free();
	gTileTexture.free();
	gTextCoordinates.free();
	gTextVelocity.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

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

			/*
			gTileClips[TILE_RED].x = 0;
			gTileClips[TILE_RED].y = 0;
			gTileClips[TILE_RED].w = TILE_WIDTH;
			gTileClips[TILE_RED].h = TILE_HEIGHT;

			gTileClips[TILE_GREEN].x = 0;
			gTileClips[TILE_GREEN].y = 80;
			gTileClips[TILE_GREEN].w = TILE_WIDTH;
			gTileClips[TILE_GREEN].h = TILE_HEIGHT;

			gTileClips[TILE_BLUE].x = 0;
			gTileClips[TILE_BLUE].y = 160;
			gTileClips[TILE_BLUE].w = TILE_WIDTH;
			gTileClips[TILE_BLUE].h = TILE_HEIGHT;

			gTileClips[TILE_TOPLEFT].x = 80;
			gTileClips[TILE_TOPLEFT].y = 0;
			gTileClips[TILE_TOPLEFT].w = TILE_WIDTH;
			gTileClips[TILE_TOPLEFT].h = TILE_HEIGHT;

			gTileClips[TILE_LEFT].x = 80;
			gTileClips[TILE_LEFT].y = 80;
			gTileClips[TILE_LEFT].w = TILE_WIDTH;
			gTileClips[TILE_LEFT].h = TILE_HEIGHT;

			gTileClips[TILE_BOTTOMLEFT].x = 80;
			gTileClips[TILE_BOTTOMLEFT].y = 160;
			gTileClips[TILE_BOTTOMLEFT].w = TILE_WIDTH;
			gTileClips[TILE_BOTTOMLEFT].h = TILE_HEIGHT;

			gTileClips[TILE_TOP].x = 160;
			gTileClips[TILE_TOP].y = 0;
			gTileClips[TILE_TOP].w = TILE_WIDTH;
			gTileClips[TILE_TOP].h = TILE_HEIGHT;

			gTileClips[TILE_CENTER].x = 160;
			gTileClips[TILE_CENTER].y = 80;
			gTileClips[TILE_CENTER].w = TILE_WIDTH;
			gTileClips[TILE_CENTER].h = TILE_HEIGHT;

			gTileClips[TILE_BOTTOM].x = 160;
			gTileClips[TILE_BOTTOM].y = 160;
			gTileClips[TILE_BOTTOM].w = TILE_WIDTH;
			gTileClips[TILE_BOTTOM].h = TILE_HEIGHT;

			gTileClips[TILE_TOPRIGHT].x = 240;
			gTileClips[TILE_TOPRIGHT].y = 0;
			gTileClips[TILE_TOPRIGHT].w = TILE_WIDTH;
			gTileClips[TILE_TOPRIGHT].h = TILE_HEIGHT;

			gTileClips[TILE_RIGHT].x = 240;
			gTileClips[TILE_RIGHT].y = 80;
			gTileClips[TILE_RIGHT].w = TILE_WIDTH;
			gTileClips[TILE_RIGHT].h = TILE_HEIGHT;

			gTileClips[TILE_BOTTOMRIGHT].x = 240;
			gTileClips[TILE_BOTTOMRIGHT].y = 160;
			gTileClips[TILE_BOTTOMRIGHT].w = TILE_WIDTH;
			gTileClips[TILE_BOTTOMRIGHT].h = TILE_HEIGHT;
			*/
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
		if(tiles[i]->getType() % 4 != 0 && tiles[i]->getType() < 12) {
			//If the collision box touches the wall tile
			if(checkCollision(box, tiles[i]->getBox())) {
				return i;
			}
		}
	}

	//If no wall tiles were touched
	return -1;
}

int main(int argc, char *args[]) {
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

			// Timer.
			LTimer stepTimer;

			//Level camera
			SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

			float acceleration = (1.07 * 60);
			bool toggleParticles = true;
			SDL_Color textColor = {136, 0, 21};
			std::ostringstream os;

			//While application is running
			while(!quit) {
				//Handle events on queue
				while(SDL_PollEvent(&e) != 0) {
					//User requests quit
					if(e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE) {
						quit = true;
					}
					if(e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_1) {
						setTiles(tileSet, "lazy2.map");
					}
					if(e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_2) {
						setTiles(tileSet, "lazy.map");
					}
					if(e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_3) {
						toggleParticles = !toggleParticles;
					}

					// input for the dot
					dot.handleEvent(e);
				}

				// Calculate time step.
				float timeStep = stepTimer.getTicks() / 1000.f; //1.0;
				std::cout << timeStep << std::endl;

				// Gravity implementation.
				if(dot.getVelocityY() < 900) {
					dot.setVelocityY(dot.getVelocityY() + acceleration);
				}
				if(dot.getVelocityY() > 900) {
					dot.setVelocityY(900);
				}
				if(dot.getVelocityY() < -1200) {
					dot.setVelocityY(-1200);
				}

				/*
				if(dot.getVelocityY() < 15) {
					dot.setVelocityY(dot.getVelocityY() + acceleration);
				}
				if(dot.getVelocityY() > 15) {
					dot.setVelocityY(15);
				}
				if(dot.getVelocityY() < -20) {
					dot.setVelocityY(-20);
				}
				*/

				//Move the dot.
				dot.move(tileSet, timeStep);
				dot.setCamera(camera);

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

				//Render level
				for(int i = 0; i < TOTAL_TILES; ++i) {
					tileSet[i]->render(camera);
				}

				// Render font.
				gTextCoordinates.render((SCREEN_WIDTH - gTextCoordinates.getWidth()),  0);
				gTextVelocity.render((SCREEN_WIDTH - gTextVelocity.getWidth()),  30);

				//Render dot
				dot.render(camera, toggleParticles);

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}

		//Free resources and close SDL
		close(tileSet);
	}

	return 0;
}
