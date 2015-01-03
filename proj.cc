#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "npc.hpp"
#include "texture.hpp"
#include "tiles.hpp"
#include "particle.hpp"
#include "timer.hpp"
#include "button.hpp"
#include "character.hpp"
#include "globals.hpp"

//The window we'll be rendering to
SDL_Window *gWindow;

//The window renderer
SDL_Renderer *gRenderer; 

// Globally used font.
TTF_Font *gFont = NULL;
LTexture gTextCoordinates;
LTexture gFpsTextTexture;
LTexture gTextVelocity;
LTexture gMouseCoordinates;

float gScale;
float gCharacterWidthScale;
float gCharacterHeightScale;
int gCharacterFrameRate;

LTexture gButtonSpriteSheetTexture;
LButton gButtons[TOTAL_BUTTONS];
SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];

//Scene textures
LTexture gTileTexture;
SDL_Rect gTileClips[TOTAL_TILE_SPRITES];

LTexture gBGTexture;

LTexture gRedTexture;
LTexture gGreenTexture;
LTexture gBlueTexture;
LTexture gShimmerTexture;

Mix_Music *gMusic[4];

std::ofstream logger;

//int counter = 0;

void log(std::string message) {
	// Write message to file.
	logger << message << " " << std::endl;

	// Flush the buffer.
	logger.flush();
}

//The character that will move around on the screen
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia(Tile *tiles[]);

//Frees media and shuts down SDL
void close(Tile *tiles[]);

//Sets tiles from tile map
bool setTiles(Tile *tiles[], std::string mapName);

//Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

//Checks collision box against set of tiles
int touchesWall(SDL_Rect box, Tile *tiles[]);
bool touchesTap(SDL_Rect box, Tile *tiles[]);

int touchesNpc(SDL_Rect box, std::vector<Npc *> &npcVector);

bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Set texture filtering to linear
		if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Awesome Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

				// Initialize SDL_mixer.
				if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
					printf("SDL_mixer failed to initialize, error: %s\n", Mix_GetError());
					success = false;
				}
			}
		}
	}

	// load configuration file.
	std::ifstream config("config.txt");
	if(config == NULL) {
		printf("unable to load config file.");
		success = false;
	}
	else {
		std::string tmp;
		config >> tmp;
		config >> gScale;
		config >> tmp;
		config >> gCharacterFrameRate;
		config >> tmp;
		config >> gCharacterWidthScale;
		config >> tmp;
		config >> gCharacterHeightScale;
		config.close();
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
	//Load sprites
	if(!gButtonSpriteSheetTexture.loadFromFile("button.png")) {
		printf( "Failed to load button sprite texture!\n" );
		success = false;
	}
	else {
		//Set sprites
		for( int i = 0; i < BUTTON_SPRITE_TOTAL; ++i )
		{
			gSpriteClips[i].x = 0;
			gSpriteClips[i].y = i * 200;
			gSpriteClips[i].w = BUTTON_WIDTH;
			gSpriteClips[i].h = BUTTON_HEIGHT;
		}

		//Set buttons in corners
		gButtons[0].setPosition(gButtons[0].dstrect.x, gButtons[0].dstrect.y);
	}

	// Load music.
	gMusic[0] = Mix_LoadMUS("tokage.mid");
	if(gMusic[0] == NULL) {
		printf("failed to load music, error: %s\n", Mix_GetError());
		success = false;
	}
	gMusic[1] = Mix_LoadMUS("wild.mid");
	if(gMusic[1] == NULL) {
		printf("failed to load music, error: %s\n", Mix_GetError());
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
	log("killing particle textures...");
	gRedTexture.free();
	gBlueTexture.free();
	gGreenTexture.free();
	gShimmerTexture.free();
	log("killing tile textures...");
	gTileTexture.free();
	log("killing font textures...");
	gTextCoordinates.free();
	gTextVelocity.free();
	gMouseCoordinates.free();
	gFpsTextTexture.free();
	log("killing background/mouse textures...");
	gBGTexture.free();
	gButtonSpriteSheetTexture.free();

	// Free music.
	log("killing music...");
	Mix_FreeMusic(gMusic[1]);
	gMusic[1] = NULL;
	Mix_FreeMusic(gMusic[0]);
	gMusic[0] = NULL;
	

	//Destroy window	
	log("killing window/renderer/font...");
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	gFont = NULL;

	//Quit SDL subsystems
	log("quitting subsystems...");
	Mix_Quit();
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

bool checkUpperCollision(SDL_Rect a, SDL_Rect b) {
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
	topB = b.y - 50;
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
			for(int i = 0; i < TOTAL_TILE_SPRITES / 2; ++i) {
				gTileClips[i].x = x;
				gTileClips[i].y = y;
				gTileClips[i].w = TILE_WIDTH;
				gTileClips[i].h = TILE_HEIGHT;

				x += TILE_WIDTH;
				if(x >=  TILESHEET_WIDTH / 2) {
					x = 0;
					y += TILE_HEIGHT;
				}
			}

			x = TILESHEET_WIDTH / 2;
			y = 0;
			for(int i = TOTAL_TILE_SPRITES / 2; i < TOTAL_TILE_SPRITES; ++i) {
				gTileClips[i].x = x;
				gTileClips[i].y = y;
				gTileClips[i].w = TILE_WIDTH;
				gTileClips[i].h = TILE_HEIGHT;

				x += TILE_WIDTH;
				if(x >=  TILESHEET_WIDTH) {
					x = TILESHEET_WIDTH / 2;
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

bool touchesTap(SDL_Rect box, Tile *tiles[]) {
	//Go through the tiles
	for(int i = 0; i < TOTAL_TILES; ++i) {
		//If the tile is a wall type tile
		if(tiles[i]->getType() % 4 != 0 && tiles[i]->getType() < 20) {
			//If the collision box touches the wall tile
			if(checkUpperCollision(box, tiles[i]->getBox())) {
				return true;
			}
		}
	}

	//If no wall tiles were touched
	return false;
}

int touchesWall(SDL_Rect box, Tile *tiles[]) {
	//Go through the tiles
	for(int i = 0; i < TOTAL_TILES; ++i) {
		//If the tile is a wall type tile
		if((tiles[i]->getType() % 4 != 0 && tiles[i]->getType() < 20) || 
			 (tiles[i]->getType() % 4 != 0 && tiles[i]->getType() >= 49 && tiles[i]->getType() < 68) ||
			 (tiles[i]->getType() % 4 != 0 && tiles[i]->getType() > 20 && tiles[i]->getType() <= 27 && tiles[i]->getType() != 23)) {
			//If the collision box touches the wall tile
			if(tiles[i]->getType() > 20 && tiles[i]->getType() <= 27) {
				if(checkCollision(box, tiles[i]->getCollisionBox())) {
					return i;
				}
			}
			else {
				if(checkCollision(box, tiles[i]->getBox())) {
					return i;
				}
			}
		}
	}

	//If no wall tiles were touched
	return -1;
}

int touchesNpc(SDL_Rect box, std::vector<Npc *> &npcVector) {
	//Go through the npc
	for(unsigned int i = 0; i < npcVector.size(); ++i) {
		if(checkCollision(box, npcVector[i]->getBoxPosition())) {
			return i;
		}
	}

	//If no wall tiles were touched
	return -1;
}

int main(int argc, char *args[]) {
	
restart:

	logger.open("log.txt");
	bool restart = false;
	//Start up SDL and create window
	log("initializing...");
	if(!init()) {
		printf("Failed to initialize!\n");
	}
	else {
		//The level tiles
		Tile *tileSet[TOTAL_TILES];

		//Load media
		log("loading files...");
		if(!loadMedia(tileSet)) {
			printf("Failed to load media!\n");
		}

		else {

			log("initializing variables...");
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The character that will be moving around on the screen
			Character character((int) (37 * gCharacterWidthScale), (int) (48 * gCharacterHeightScale));
			character.frameRate = gCharacterFrameRate;

			//vector implementation
			std::vector<Npc *> npcVector;
			npcVector.push_back(new Npc(rand() % (LEVEL_WIDTH - (int) (38 * gScale)) + TILE_WIDTH, 0, (int) (38 * gScale), (int) (55 * gScale), 4, "character2.png"));
			npcVector.push_back(new Npc(rand() % (LEVEL_WIDTH - (int) (38 * gScale)) + TILE_WIDTH, 0, (int) (38 * gScale), (int) (55 * gScale), 4, "character2.png"));
			npcVector.push_back(new Npc(rand() % (LEVEL_WIDTH - (int) (38 * gScale)) + TILE_WIDTH, 0, (int) (38 * gScale), (int) (55 * gScale), 4, "character3.png"));
			npcVector.push_back(new Npc(rand() % (LEVEL_WIDTH - (int) (38 * gScale)) + TILE_WIDTH, 0, (int) (38 * gScale), (int) (55 * gScale), 4, "character1.png"));
			npcVector.push_back(new Npc(rand() % (LEVEL_WIDTH - (int) (76 * gScale)) + TILE_WIDTH, 0, (int) (76 * gScale), (int) (105 * gScale), 4, "character4.png"));

			npcVector[4]->spriteClips[0].x = 0;
			npcVector[4]->spriteClips[0].y = 0;
			npcVector[4]->spriteClips[0].w = 76;
			npcVector[4]->spriteClips[0].h = 105;

			npcVector[4]->spriteClips[1].x = 77;
			npcVector[4]->spriteClips[1].y = 0;
			npcVector[4]->spriteClips[1].w = 96;
			npcVector[4]->spriteClips[1].h = 105;

			npcVector[4]->spriteClips[2].x = 174;
			npcVector[4]->spriteClips[2].y = 0;
			npcVector[4]->spriteClips[2].w = 75;
			npcVector[4]->spriteClips[2].h = 105;

			npcVector[4]->spriteClips[3].x = 250;
			npcVector[4]->spriteClips[3].y = 0;
			npcVector[4]->spriteClips[3].w = 96;
			npcVector[4]->spriteClips[3].h = 105;

			// Timer.
			LTimer stepTimer;

			//Level camera
			SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

			float avgFPS;
			bool toggleParticles = true;
			SDL_Color textColor = {136, 0, 21};
			std::stringstream os;

			std::stringstream timeText;
			// Frames per second timer.
			LTimer fpsTimer;

			// Frames per second cap timer.
			LTimer capTimer;

			// Npc timer.
			LTimer npcTimer;
			LTimer animationTimer;
			//unsigned int oldTimer = 0;

			// Start timer.
			int countedFrames = 0;

			// Current animation frame.
			Uint32 frame = 0;

			// Background scrolling offset.
			int scrollingOffset = 0;

			std::string randomGuy;
			/*
			SDL_Rect wholeScreenViewport;
			wholeScreenViewport.x = 0;
			wholeScreenViewport.y = 0;
			wholeScreenViewport.w = SCREEN_WIDTH;
			wholeScreenViewport.h = SCREEN_HEIGHT;

			SDL_Rect topLeftViewport;
			topLeftViewport.x = 50;
			topLeftViewport.y = 50;
			topLeftViewport.w = 100;
			topLeftViewport.h = 100;
			*/

			int xMouse, yMouse;
			fpsTimer.start();
			npcTimer.start();
			animationTimer.start();

			Uint32 ticks;
			Uint32 seconds;

			log("beginning main loop...");
			//While application is running
			while(!quit && !restart) {

				// introduce lag.
				//system("./clear.sh");

				ticks = SDL_GetTicks();

				log("in main loop...");

				// Start cap timer.
				capTimer.start();

				//Handle events on queue

				log("handling events...");
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

					// Handle button events.
					for(int i = 0; i < TOTAL_BUTTONS; ++i) {
						gButtons[i].handleEvent(&e);
					}

					if(e.type == SDL_MOUSEBUTTONDOWN) {
						os.str("");
						int random = rand() % 4 + 1;
						os << "character" << random << ".png";
						if(random == 4) {
							npcVector.push_back(new Npc(camera.x + xMouse, camera.y + yMouse, (int)(76 * gScale), (int)(105 * gScale), 4, os.str()));

							npcVector[npcVector.size() - 1]->spriteClips[0].x = 0;
							npcVector[npcVector.size() - 1]->spriteClips[0].y = 0;
							npcVector[npcVector.size() - 1]->spriteClips[0].w = 76;
							npcVector[npcVector.size() - 1]->spriteClips[0].h = 105;

							npcVector[npcVector.size() - 1]->spriteClips[1].x = 77;
							npcVector[npcVector.size() - 1]->spriteClips[1].y = 0;
							npcVector[npcVector.size() - 1]->spriteClips[1].w = 96;
							npcVector[npcVector.size() - 1]->spriteClips[1].h = 105;

							npcVector[npcVector.size() - 1]->spriteClips[2].x = 174;
							npcVector[npcVector.size() - 1]->spriteClips[2].y = 0;
							npcVector[npcVector.size() - 1]->spriteClips[2].w = 75;
							npcVector[npcVector.size() - 1]->spriteClips[2].h = 105;

							npcVector[npcVector.size() - 1]->spriteClips[3].x = 250;
							npcVector[npcVector.size() - 1]->spriteClips[3].y = 0;
							npcVector[npcVector.size() - 1]->spriteClips[3].w = 96;
							npcVector[npcVector.size() - 1]->spriteClips[3].h = 105;
						}
						else {
							npcVector.push_back(new Npc(camera.x + xMouse, camera.y + yMouse, (int) (gScale * 38), (int) (gScale * 55), 4, os.str()));
						}
					}

					// input for the character
					character.handleEvent(e);

					if(e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_q) {
						if(npcVector.size() != 0) {
							delete npcVector[npcVector.size() - 1];
							npcVector.pop_back();
						}
					}
				}

				// Calculate time step.
				float timeStep = stepTimer.getTicks() / 1000.f; //1.0;

				if(character.headJump == true) {
					character.setVelocityY(0);
					character.setVelocityY(-character.CHARACTER_VELY);
					character.headJump = false;
				}

				seconds = ticks / 1000.f;
				if(seconds % 10 == 0) {
					logger.close();
					logger.open("log.txt");
				}

				// Calculate and correct fps.
				avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
				if(avgFPS > 2000000) avgFPS = 0;

				// FPS text.
				timeText.str("");
				timeText << "FPS: " << avgFPS;
				if(!gFpsTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor)) {
					printf("unable to render FPS texture\n");
				}

				
				for(unsigned int i = 0; i < npcVector.size(); ++i) {
					if(npcVector[i]->wasStabbed) {
						if(npcVector[i]->wasAttackedTimer.getTicks() > 300) {
							npcVector[i]->setVelocityX(0);
							npcVector[i]->wasStabbed = false;
							npcVector[i]->wasAttackedTimer.stop();
						}
					}
				}

				// AI.
				if((npcTimer.getTicks() / 1000) != 0 && (npcTimer.getTicks() / 1000) % 2  == 0) {
					for(unsigned int i = 0; i < npcVector.size(); ++i) {
						if(!npcVector[i]->wasStabbed) {
							switch(rand() % 3) {
								case 0:
									npcVector[i]->isMoving = true;
									npcVector[i]->setVelocityX(-npcVector[i]->NPC_VELX);
									npcVector[i]->flip = SDL_FLIP_NONE;
									break;
								case 1:
									npcVector[i]->isMoving = true;
									npcVector[i]->setVelocityX(npcVector[i]->NPC_VELX);
									npcVector[i]->flip = SDL_FLIP_HORIZONTAL;
									break;
								case 2:
									npcVector[i]->isMoving = false;
									npcVector[i]->setVelocityX(0);
									break;
								default:
									break;
							}
						}
					}
					npcTimer.start();
				}

        // Whole screen viewport.
        //SDL_RenderSetViewport(gRenderer, &wholeScreenViewport);

				//Move the character.
				log("moving character...");
				character.move(tileSet, npcVector, timeStep /*1 for now*/);

				// Restart step timer.
				stepTimer.start();
				
				for(unsigned int i = 0; i < npcVector.size(); ++i) {
					npcVector[i]->move(tileSet, character, timeStep /*1 for now*/);
					if(npcVector[i]->wasJumped) {
						delete npcVector[i];
						npcVector.erase(npcVector.begin() + i);
					}
					//if(npcVector[i]->wasStabbed) {
						//npcVector[i]->setVelocityX(15 * 60);
					//}
				}

				log("setting camera...");
				character.setCamera(camera);

				// Scroll background.
				--scrollingOffset;
				if(scrollingOffset < -gBGTexture.getWidth()) {
					scrollingOffset = 0;
				}

				log("preparing font info...");
				os.str("");
				os << character.getBoxPosition().x << ", " << character.getBoxPosition().y;
				if(!gTextCoordinates.loadFromRenderedText(os.str().c_str(), textColor)) {
					log("error!");
					printf("failed to render text texture\n");
				}

				os.str("");
				os << character.getVelocityX() << ", " << (int) character.getVelocityY();
				if(!gTextVelocity.loadFromRenderedText(os.str().c_str(), textColor)) {
					log("error!");
					printf("failed to render text texture\n");
				}

				os.str("");
				SDL_GetMouseState(&xMouse, &yMouse);
				os << xMouse << ", " << yMouse << ": " << npcVector.size();
				if(!gMouseCoordinates.loadFromRenderedText(os.str().c_str(), textColor)) {
					log("error!");
					printf("failed to render text texture\n");
				}

				log("clearing screen...");
				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				gBGTexture.render(scrollingOffset, 0);
				gBGTexture.render(scrollingOffset + gBGTexture.getWidth(), 0);

				//Render level
				log("rendering level...");
				for(int i = 0; i < TOTAL_TILES; ++i) {
					tileSet[i]->render(camera);
				}

				// Render font.
				log("rendering font...");
				gTextCoordinates.render((SCREEN_WIDTH - gTextCoordinates.getWidth()),  0);
				gTextVelocity.render((SCREEN_WIDTH - gTextVelocity.getWidth()),  30);
				gFpsTextTexture.render((SCREEN_WIDTH - gFpsTextTexture.getWidth()),  60);
				gMouseCoordinates.render((SCREEN_WIDTH - gMouseCoordinates.getWidth()),  90);

				//Render character
				//Uint32 sprite = seconds % 4;
				frame = (ticks / 100) % 4;

				log("rendering character...");
				character.render(camera, toggleParticles, gCharacterWidthScale, gCharacterHeightScale);

				log("rendering npc...");
				
				for(unsigned int i = 0; i < npcVector.size(); ++i) {
					if(npcVector[i]->isMoving || npcVector[i]->NPC_HEIGHT == (int)(105 * gScale)) {
						npcVector[i]->currentClip = &npcVector[i]->spriteClips[frame];
					}
					else { 
						npcVector[i]->currentClip = &npcVector[i]->spriteClips[1]; 
					}
					if(npcVector[i]->NPC_HEIGHT == (int)(105 * gScale)) npcVector[i]->render(camera, toggleParticles, npcVector[i]->currentClip, gScale);
					else npcVector[i]->render(camera, toggleParticles, npcVector[i]->currentClip, gScale);
				}
				
				// Next frame.
				// XXX BTON U ?LEFT OFF HERE.
				//++frame;
				// Cycle.
				//if(frame / character.ANIMATION_FRAMES >= character.ANIMATION_FRAMES) frame = 0;

        //SDL_RenderSetViewport(gRenderer, &topLeftViewport);
				// Render buttons.
				for(int i = 0; i < TOTAL_BUTTONS; ++i) {
					gButtons[i].render();
				}

				log("updating screen...");
				//Update screen
				SDL_RenderPresent(gRenderer);
				++countedFrames;

				// If frame time finished early.
				int frameTicks = capTimer.getTicks();
				if(frameTicks < SCREEN_TICKS_PER_FRAME) {
					// Wait remaining time.
					SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
				}

				log("end loop...");
			}

			if(quit) log("quit");
			else log("false");

			log("freeing...");
			character.characterTexture.free();
			/*
			for(int i = 0; i < contained; ++i) {
				delete npcContainer[i];
			}
			*/

			for(unsigned int i = 0; i < npcVector.size(); ++i) {
				delete npcVector[i];
			}
			npcVector.clear();
		}

		//Free resources and close SDL
		log("closing...");
		close(tileSet);
		log("after...");
		if(restart) goto restart;
	}

	log("shutdown...");
	logger.close();
	return 0;
}
