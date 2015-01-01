#ifndef GLOBALS_HPP
	#define GLOBALS_HPP
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <SDL_mixer.h>
#include <vector>
#include "texture.hpp"

class LTexture;
class LButton;
class Npc;

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

//The dimensions of the level
const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960 * 2;

// Frame rate cap.
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

//Tile constants
const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 192 * 2;
const int TOTAL_TILE_SPRITES = 48;

//The different tile sprites
const int TILESHEET_WIDTH = 320;
const int TILESHEET_HEIGHT = 960;

const int TOTAL_PARTICLES = 15;

const int TOTAL_NPCS = 100;

// Button constants.
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 1;

//The window we'll be rendering to
extern SDL_Window *gWindow;

//The window renderer
extern SDL_Renderer *gRenderer;

// Font.
extern TTF_Font *gFont;

extern bool checkCollision(SDL_Rect a, SDL_Rect b);
extern LTexture gTileTexture;
extern SDL_Rect gTileClips[TOTAL_TILE_SPRITES];

extern LTexture gRedTexture;
extern LTexture gGreenTexture;
extern LTexture gBlueTexture;
extern LTexture gShimmerTexture;
extern LTexture gButtonSpriteSheetTexture;

extern LButton gButtons[TOTAL_BUTTONS];
extern SDL_Rect gSpriteClips[4];

extern void log(std::string message);
extern int touchesNpc(SDL_Rect box, std::vector<Npc *> &npcContainer);
extern Mix_Music *gMusic[4];

#endif
