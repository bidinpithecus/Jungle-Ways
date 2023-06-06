#ifndef _GAME_HPP_
#define _GAME_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

enum class GAME_STATE {
	IN_GAME_MENU,
	MAIN_MENU,
	OPTIONS_SUB_MENU,
	PLAYING
};

class Game {
public:
	Game(int width, int height);
	int OnExecute();
	bool OnInit();
	void OnEvent(SDL_Event* event);
	void OnLoop();
	void OnRender();
	void OnExit();
	void RenderMainMenu();
	void RenderOptionsMenu();
	void RenderInGameMenu();
	void RenderScene();

private:
	int width;
	int height;

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
};

#endif
