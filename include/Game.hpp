#ifndef _GAME_HPP_
#define _GAME_HPP_

#include <SDL2/SDL.h>
#include <iostream>

class Game {
public:
	Game(int width, int height);
	int OnExecute();
	bool OnInit();
	void OnEvent(SDL_Event* event);
	void OnLoop();
	void OnRender();
	void OnExit();

private:
	int width;
	int height;

	bool isRunning;
	SDL_GLContext glContext;
	SDL_Renderer* pRenderer;
	SDL_Window* pWindow;
};

#endif
