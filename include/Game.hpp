#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Utils.hpp"
#include "physics/Body.hpp"
#include "physics/World.hpp"

#include <GL/gl.h>
#include <GL/glu.h>
#include <random>
#include <ctime>

enum struct TEXTURE {
	branch = 0,
	character = 1,
	tree = 2
};

enum class GAME_STATE {
	IN_GAME_MENU,
	MAIN_MENU,
	OPTIONS_SUB_MENU,
	PLAYING
};

class Game {
public:
	Game(int width, int height);

	void handleCharacter();
	void LevelDesign(bool isRandom);
	void Logic();
	void OnEvent(SDL_Event* event);
	int OnExecute();
	void OnExit();
	bool OnInit();
	void OnLoop();
	void OnRender();
	void RenderInGameMenu();
	void RenderMainMenu();
	void RenderMenuOption(const char* optionText, int x, int y, int width, int height, SDL_Color textColor);
	void RenderOptionsMenu();
	void RenderScene();
	void ResetGame(bool isRandom);

private:
	int width;
	int height;
    const float IN_GAME_MENU_WIDTH_RATIO = 0.33f;
    const float IN_GAME_MENU_HEIGHT_RATIO = 0.80f;
    const float MAIN_MENU_WIDTH_RATIO = 0.90f;
    const float MAIN_MENU_HEIGHT_RATIO = 0.90f;
	const float tick = 1 / 60.0f;
	bool isRunning;
	GAME_STATE gameState;
	GAME_STATE previousState;

	SDL_GLContext glContext;
	TTF_Font* pFont;
	SDL_Renderer* pRenderer;
	SDL_Window* pWindow;
	SDL_Rect startGameRect;
    SDL_Rect optionsRect;
    SDL_Rect exitRect;

	const Uint8* keyboardStateArray = SDL_GetKeyboardState(NULL);

	physics::World world;

	physics::Body* initialTree;
	physics::Body* finalTree;
	physics::Body* initialBranch;
	physics::Body* finalBranch;
	physics::Body* anotherBranch;
	physics::Body* character;
};
