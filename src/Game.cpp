#include "../include/Game.hpp"

// Default constructor
Game::Game(int width, int height) {
	this->width = width;
	this->height = height;

	isRunning = true;
	gameState = GAME_STATE::MAIN_MENU;
	previousState = gameState;
	pWindow = NULL;
	pRenderer = NULL;
	glContext = NULL;
	pFont = NULL;
}

bool Game::OnInit() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) return false;

	pWindow = SDL_CreateWindow("Jungle Ways", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (!pWindow) return false;

	pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!pRenderer) return false;

	if (TTF_Init() < 0) return false;

	pFont = TTF_OpenFont("../assets/fonts/FiraCode_SemiBold.ttf", 24);
	if (!pFont) return false;

	return true;
}

int Game::OnExecute() {
	SDL_Event event;
	if (!OnInit()) return -1;

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
	if (event->type == SDL_QUIT) {
		isRunning = false;
	}

	if (gameState == GAME_STATE::IN_GAME_MENU) {
		previousState = GAME_STATE::IN_GAME_MENU;
		if (event->type == SDL_KEYDOWN) {
			if (event->key.keysym.sym == SDLK_1) {
				gameState = GAME_STATE::PLAYING;
			} else if (event->key.keysym.sym == SDLK_2) {
				gameState = GAME_STATE::OPTIONS_SUB_MENU;
			} else if (event->key.keysym.sym == SDLK_3) {
				gameState = GAME_STATE::MAIN_MENU;
			}
		}
	} else if (gameState == GAME_STATE::MAIN_MENU) {
		previousState = GAME_STATE::MAIN_MENU;
		if (event->type == SDL_KEYDOWN) {
			if (event->key.keysym.sym == SDLK_1) {
				gameState = GAME_STATE::PLAYING;
			} else if (event->key.keysym.sym == SDLK_2) {
				gameState = GAME_STATE::OPTIONS_SUB_MENU;
			} else if (event->key.keysym.sym == SDLK_3) {
				isRunning = false;
			}
		}
	} else if (gameState == GAME_STATE::OPTIONS_SUB_MENU) {
		if (event->type == SDL_KEYDOWN) {
			if (event->key.keysym.sym == SDLK_1) {
				std::cout << "Sub-option 1 chosen" << std::endl;
			} else if (event->key.keysym.sym == SDLK_2) {
				std::cout << "Sub-option 2 chosen" << std::endl;
			} else if (event->key.keysym.sym == SDLK_3) {
				if (previousState == GAME_STATE::MAIN_MENU) {
					gameState = GAME_STATE::MAIN_MENU;
				} else if (previousState == GAME_STATE::IN_GAME_MENU) {
					gameState = GAME_STATE::IN_GAME_MENU;
				}
			}
		}
	} else if (gameState == GAME_STATE::PLAYING) {
		previousState = GAME_STATE::PLAYING;
		if (event->type == SDL_KEYDOWN) {
			if (event->key.keysym.sym == SDLK_ESCAPE) {
				gameState = GAME_STATE::IN_GAME_MENU;
			}
		}
	}
}

void Game::OnLoop() {
	// Perform game logic here
}

void Game::OnRender() {
	SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
	SDL_RenderClear(pRenderer);

	if (gameState == GAME_STATE::IN_GAME_MENU) {
		RenderInGameMenu();
	} else if (gameState == GAME_STATE::OPTIONS_SUB_MENU) {
		RenderOptionsMenu();
	} else if (gameState == GAME_STATE::MAIN_MENU) {
		RenderMainMenu();
	} else if (gameState == GAME_STATE::PLAYING) {
		RenderScene();
	}

	SDL_RenderPresent(pRenderer);
}

void Game::OnExit() {
	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWindow);
	pWindow = NULL;
	SDL_Quit();
}

void Game::RenderMainMenu() {
	SDL_Color textColor = { 0, 0, 0, 255 };

	SDL_Surface* surface = NULL;
	SDL_Texture* texture = NULL;

	// Start Game button
	const char* startGameText = "Start Game";
	surface = TTF_RenderText_Solid(pFont, startGameText, textColor);
	texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	int textWidth = surface->w;
	int textHeight = surface->h;
	SDL_FreeSurface(surface);
	startGameRect = { (width - textWidth) / 2, (height - textHeight) / 2 - 50, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &startGameRect);
	SDL_DestroyTexture(texture);

	// Options button
	const char* optionsText = "Options";
	surface = TTF_RenderText_Solid(pFont, optionsText, textColor);
	texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	textWidth = surface->w;
	textHeight = surface->h;
	SDL_FreeSurface(surface);
	optionsRect = { (width - textWidth) / 2, (height - textHeight) / 2, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &optionsRect);
	SDL_DestroyTexture(texture);

	// Exit button
	const char* exitText = "Exit Game";
	surface = TTF_RenderText_Solid(pFont, exitText, textColor);
	texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	textWidth = surface->w;
	textHeight = surface->h;
	SDL_FreeSurface(surface);
	exitRect = { (width - textWidth) / 2, (height - textHeight) / 2 + 50, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &exitRect);
	SDL_DestroyTexture(texture);
}

void Game::RenderOptionsMenu() {
	SDL_Color textColor = { 0, 0, 0, 255 };

	SDL_Surface* surface = NULL;
	SDL_Texture* texture = NULL;

	// Start Game button
	const char* soundText = "Ambient Sound";
	surface = TTF_RenderText_Solid(pFont, soundText, textColor);
	texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	int textWidth = surface->w;
	int textHeight = surface->h;
	SDL_FreeSurface(surface);
	startGameRect = { (width - textWidth) / 2, (height - textHeight) / 2 - 50, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &startGameRect);
	SDL_DestroyTexture(texture);

	// Options button
	const char* musicText = "Music";
	surface = TTF_RenderText_Solid(pFont, musicText, textColor);
	texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	textWidth = surface->w;
	textHeight = surface->h;
	SDL_FreeSurface(surface);
	optionsRect = { (width - textWidth) / 2, (height - textHeight) / 2, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &optionsRect);
	SDL_DestroyTexture(texture);

	// Exit button
	const char* returnText = "Go back";
	surface = TTF_RenderText_Solid(pFont, returnText, textColor);
	texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	textWidth = surface->w;
	textHeight = surface->h;
	SDL_FreeSurface(surface);
	exitRect = { (width - textWidth) / 2, (height - textHeight) / 2 + 50, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &exitRect);
	SDL_DestroyTexture(texture);
}

void Game::RenderInGameMenu() {
	SDL_Color textColor = { 0, 0, 0, 255 };

	SDL_Surface* surface = NULL;
	SDL_Texture* texture = NULL;

	// Start Game button
	const char* startGameText = "Resume Game";
	surface = TTF_RenderText_Solid(pFont, startGameText, textColor);
	texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	int textWidth = surface->w;
	int textHeight = surface->h;
	SDL_FreeSurface(surface);
	startGameRect = { (width - textWidth) / 2, (height - textHeight) / 2 - 50, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &startGameRect);
	SDL_DestroyTexture(texture);

	// Options button
	const char* optionsText = "Options";
	surface = TTF_RenderText_Solid(pFont, optionsText, textColor);
	texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	textWidth = surface->w;
	textHeight = surface->h;
	SDL_FreeSurface(surface);
	optionsRect = { (width - textWidth) / 2, (height - textHeight) / 2, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &optionsRect);
	SDL_DestroyTexture(texture);

	// Exit button
	const char* mainMenuText = "Back to Main Menu";
	surface = TTF_RenderText_Solid(pFont, mainMenuText, textColor);
	texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	textWidth = surface->w;
	textHeight = surface->h;
	SDL_FreeSurface(surface);
	exitRect = { (width - textWidth) / 2, (height - textHeight) / 2 + 50, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &exitRect);
	SDL_DestroyTexture(texture);
}

void Game::RenderScene() {
	// Render scene
}
