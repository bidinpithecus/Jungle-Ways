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

	pFont = TTF_OpenFont("../assets/fonts/FiraCode_SemiBold.ttf", 32);
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
	// Game logic here
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

	// Calculate menu dimensions
	int menuWidth = static_cast<int>(width * MAIN_MENU_WIDTH_RATIO);
	int menuHeight = static_cast<int>(height * MAIN_MENU_HEIGHT_RATIO);

	// Calculate menu position
	int menuX = (width - menuWidth) / 2;
	int menuY = (height - menuHeight) / 2;

	// Render the menu background
	SDL_SetRenderDrawColor(pRenderer, 192, 192, 192, 255);
	SDL_Rect menuRect = { menuX, menuY, menuWidth, menuHeight };
	SDL_RenderFillRect(pRenderer, &menuRect);

	// Render each option
	int optionCount = 3;
	int optionHeight = menuHeight / optionCount;
	int optionY = menuY;

	// Option 1: Start
	RenderMenuOption("Start", menuX, optionY, menuWidth, optionHeight, textColor);
	optionY += optionHeight;

	// Option 2: Options
	RenderMenuOption("Options", menuX, optionY, menuWidth, optionHeight, textColor);
	optionY += optionHeight;

	// Option 3: Exit the app
	RenderMenuOption("Exit", menuX, optionY, menuWidth, optionHeight, textColor);
}

void Game::RenderOptionsMenu() {
	SDL_Color textColor = { 0, 0, 0, 255 };
	int menuWidth = width;
	int menuHeight = height;

	// Calculate menu dimensions
	if (previousState == GAME_STATE::IN_GAME_MENU) {
		menuWidth = static_cast<int>(width * IN_GAME_MENU_WIDTH_RATIO);
		menuHeight = static_cast<int>(height * IN_GAME_MENU_HEIGHT_RATIO);
	} else if (previousState == GAME_STATE::MAIN_MENU) {
		menuWidth = static_cast<int>(width * MAIN_MENU_WIDTH_RATIO);
		menuHeight = static_cast<int>(height * MAIN_MENU_HEIGHT_RATIO);
	}

	// Calculate menu position
	int menuX = (width - menuWidth) / 2;
	int menuY = (height - menuHeight) / 2;

	// Render the menu background
	SDL_SetRenderDrawColor(pRenderer, 192, 192, 192, 255);
	SDL_Rect menuRect = { menuX, menuY, menuWidth, menuHeight };
	SDL_RenderFillRect(pRenderer, &menuRect);

	// Render each option
	int optionCount = 3;
	int optionHeight = menuHeight / optionCount;
	int optionY = menuY;

	// Option 1: General Sound
	RenderMenuOption("Ambient Sound", menuX, optionY, menuWidth, optionHeight, textColor);
	optionY += optionHeight;

	// Option 2: Options
	RenderMenuOption("Music", menuX, optionY, menuWidth, optionHeight, textColor);
	optionY += optionHeight;

	// Option 3: Return to the previous menu
	RenderMenuOption("Go back", menuX, optionY, menuWidth, optionHeight, textColor);
}

void Game::RenderInGameMenu() {
	SDL_Color textColor = { 0, 0, 0, 255 };

	// Calculate menu dimensions
	int menuWidth = static_cast<int>(width * IN_GAME_MENU_WIDTH_RATIO);
	int menuHeight = static_cast<int>(height * IN_GAME_MENU_HEIGHT_RATIO);

	// Calculate menu position
	int menuX = (width - menuWidth) / 2;
	int menuY = (height - menuHeight) / 2;

	// Render the menu background
	SDL_SetRenderDrawColor(pRenderer, 192, 192, 192, 255);
	SDL_Rect menuRect = { menuX, menuY, menuWidth, menuHeight };
	SDL_RenderFillRect(pRenderer, &menuRect);

	// Render each option
	int optionCount = 3;
	int optionHeight = menuHeight / optionCount;
	int optionY = menuY;

	// Option 1: Start Game
	const char* startGameText = "Resume Game";
	RenderMenuOption(startGameText, menuX, optionY, menuWidth, optionHeight, textColor);
	optionY += optionHeight;

	// Option 2: Options
	const char* optionsText = "Options";
	RenderMenuOption(optionsText, menuX, optionY, menuWidth, optionHeight, textColor);
	optionY += optionHeight;

	// Option 3: Exit
	const char* mainMenuText = "Back to Main Menu";
	RenderMenuOption(mainMenuText, menuX, optionY, menuWidth, optionHeight, textColor);
}

void Game::RenderMenuOption(const char* optionText, int x, int y, int width, int height, SDL_Color textColor) {
	SDL_Surface* surface = TTF_RenderText_Solid(pFont, optionText, textColor);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	int textWidth = surface->w;
	int textHeight = surface->h;
	SDL_FreeSurface(surface);

	// Render the option text
	SDL_Rect textRect = { x + (width - textWidth) / 2, y + (height - textHeight) / 2, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &textRect);
	SDL_DestroyTexture(texture);

	// Render the option rectangle boundaries
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
	SDL_Rect rect = { x, y, width, height };
	SDL_RenderDrawRect(pRenderer, &rect);
}

void Game::RenderScene() {
	// Render scene
}
