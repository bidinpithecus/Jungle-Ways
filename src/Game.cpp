#include "../include/Game.hpp"

// Default constructor
Game::Game(int width, int height) {
	this->width = width;
	this->height = height;

	isRunning = true;
	pWindow = NULL;
	pRenderer = NULL;
	glContext = NULL;
}

bool Game::OnInit() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		return false;
	}

	pWindow = SDL_CreateWindow("Vinebound Explorer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	if (pWindow != NULL) {
		pRenderer = SDL_CreateRenderer(pWindow, -1, 0);

	} else {
		return false;
	}

	return true;
}

int Game::OnExecute() {
	SDL_Event event;
	if (!OnInit()) {
		return -1;
	}

	while(isRunning) {
		while(SDL_PollEvent(&event)) {
			OnEvent(&event);
		}

		OnLoop();
		OnRender();
	}

	OnExit();
	return 0;
}

void Game::OnEvent(SDL_Event* event) {
	if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
		width = event->window.data1;
		height = event->window.data2;
	}
	if (event->type == SDL_QUIT) {
		isRunning = false;
	}
}

void Game::OnLoop() {
}

void Game::OnRender() {
	SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
	SDL_RenderClear(pRenderer);

	SDL_RenderPresent(pRenderer);
}

void Game::OnExit() {
	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWindow);
	pWindow = NULL;
	SDL_Quit();
}
