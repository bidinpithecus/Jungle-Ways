#include "../include/Game.hpp"
#include "../include/utils/FPSLimiter.hpp"

// Default constructor
Game::Game(int width, int height) {
	this->width = width;
	this->height = height;

	isRunning = true;
	gameState = GAME_STATE::MAIN_MENU;
	previousState = gameState;
	pWindow = nullptr;
	pRenderer = nullptr;
	glContext = NULL;
	pFont = nullptr;

	world.gravity = physics::Vec2(0, 9.81f);
	world.iterations = 10;
	world.bodies.reserve(8);
	world.joints.reserve(1);

	initialTree = new physics::Body();
	finalTree = new physics::Body();
	initialBranch = new physics::Body();
	finalBranch = new physics::Body();
	anotherBranch = new physics::Body();
	character = new physics::Body();
}

void Game::LevelDesign(bool isRandom) {
	initialBranch->set(physics::Vec2(initialTree->width.x * 2, initialTree->width.y / 20.0f), FLT_MAX);
	initialBranch->textureId = static_cast<int>(TEXTURE::branch);
	finalBranch->set(physics::Vec2(initialBranch->width.x, initialBranch->width.y), FLT_MAX);
	finalBranch->textureId = static_cast<int>(TEXTURE::branch);
	character->width.x = (initialTree->width.y / 20.0f) * 1.5f;
	character->width.y = 2.5f * character->width.x;
	character->set(character->width, 0.01f);
	character->textureId = static_cast<int>(TEXTURE::character);

	if (isRandom) {
		initialBranch->position.set(initialTree->position.x + initialTree->width.x / 2.0f + initialBranch->width.x / 2.0f, randomFloat(initialTree->position.y * 0.15f + character->width.y / 2.0f + initialBranch->width.y / 2.0f, initialTree->position.y * 1.875f));
		finalBranch->position.set(width - initialBranch->position.x, randomFloat(initialTree->position.y * 0.15f + character->width.y / 2.0f + finalBranch->width.y / 2.0f, initialTree->position.y * 1.875f));
	}

	world.add(initialBranch);
	world.add(finalBranch);

	anotherBranch->set(physics::Vec2(initialTree->width.x * 2, initialTree->width.y / 20.0f), FLT_MAX);
	anotherBranch->textureId = static_cast<int>(TEXTURE::branch);
	anotherBranch->position.set(width / 2.0f, height / 2.0f);
	anotherBranch->velocity.y = 20.0f;
	world.add(anotherBranch);

	// Character initial position
	character->friction = 2.0f;
	character->position.set(initialBranch->position.x, initialBranch->position.y - initialBranch->width.y / 2.0f - character->width.y / 2.0f);
	world.add(character);
}

void Game::ResetGame(bool isRandom) {
	world.clear();

	initialTree->set(physics::Vec2(width / 12.5f, height), FLT_MAX);
	initialTree->textureId = static_cast<int>(TEXTURE::tree);
	initialTree->position.set(width / 12.5f, height / 2.0f);
	initialTree->friction = 0;
	world.add(initialTree);

	finalTree->set(physics::Vec2(width / 12.5f, height), FLT_MAX);
	finalTree->textureId = static_cast<int>(TEXTURE::tree);
	finalTree->position.set(width - initialTree->position.x, height / 2.0f);
	finalTree->friction = 0;
	world.add(finalTree);

	LevelDesign(isRandom);
}

bool Game::OnInit() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		return false;
	}

	pWindow = SDL_CreateWindow("Jungle Ways", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (!pWindow) {
		return false;
	}

	pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!pRenderer) {
		return false;
	}

	if (TTF_Init() < 0) {
		return false;
	}

	glContext = SDL_GL_CreateContext(pWindow);
	if (!glContext) {
		return false;
	}

	pFont = TTF_OpenFont("../assets/fonts/tarzan-regular.ttf", 50);
	if (!pFont) {
		return false;
	}

	glEnable(GL_COLOR_MATERIAL);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return true;
}

void Game::handleCharacter() {
	if (keyboardStateArray[SDL_SCANCODE_A]) {
		world.bodies[character->id]->velocity.x = -20.0f;
	}
	if (keyboardStateArray[SDL_SCANCODE_D]) {
		world.bodies[character->id]->velocity.x = 20.0f;
	}
	if (keyboardStateArray[SDL_SCANCODE_SPACE] && world.bodies[character->id]->canJump) {
		world.bodies[character->id]->velocity.y = -40.0f;
	}
}

int Game::OnExecute() {
	SDL_Event event;
	if (!OnInit()) return -1;

	FPSLimiter fps(420);
	while(isRunning) {
		while((SDL_PollEvent(&event)) != 0) {
			OnEvent(&event);
		}
		handleCharacter();
		OnLoop();
		OnRender();
		fps.run();
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
				ResetGame(true);
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
				// Nothing yet
			} else if (event->key.keysym.sym == SDLK_2) {
				// Nothing yet
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
		if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
			if (event->key.keysym.sym == SDLK_ESCAPE) {
				gameState = GAME_STATE::IN_GAME_MENU;
			}
			if (event->key.keysym.sym == SDLK_r) {
				ResetGame(false);
			}
		}
	}
}

void Game::OnLoop() {
}

void Game::OnRender() {
	SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
	SDL_RenderClear(pRenderer);

	SDL_GL_MakeCurrent(pWindow, glContext);

	if (gameState == GAME_STATE::IN_GAME_MENU) {
		RenderInGameMenu();
	} else if (gameState == GAME_STATE::OPTIONS_SUB_MENU) {
		RenderOptionsMenu();
	} else if (gameState == GAME_STATE::MAIN_MENU) {
		RenderMainMenu();
	} else if (gameState == GAME_STATE::PLAYING) {
		RenderScene();
	}

	SDL_GL_SwapWindow(pWindow);
}

void Game::OnExit() {
	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_GL_DeleteContext(glContext);
	TTF_CloseFont(pFont);
	pRenderer = nullptr;
	pWindow = nullptr;
	glContext = NULL;
	pFont = nullptr;

	delete initialTree;
	delete finalTree;
	delete initialBranch;
	delete finalBranch;
	delete anotherBranch;
	delete character;
	world.clear();
	TTF_Quit();
	SDL_Quit();
}

void Game::RenderMainMenu() {
	glClear(GL_COLOR_BUFFER_BIT);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	SDL_Color textColor = {0, 0, 0, 255};

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
	SDL_Color textColor{0, 0, 0, 255};
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
	int menuX = static_cast<int>((width - menuWidth) / 2.0f);
	int menuY = static_cast<int>((height - menuHeight) / 2.0f);

	// Render the menu background
	SDL_SetRenderDrawColor(pRenderer, 192, 192, 192, 255);
	SDL_Rect menuRect{ menuX, menuY, menuWidth, menuHeight };
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
	SDL_Color textColor = {0, 0, 0, 255};

	// Calculate menu dimensions
	int menuWidth = static_cast<int>(width * IN_GAME_MENU_WIDTH_RATIO);
	int menuHeight = static_cast<int>(height * IN_GAME_MENU_HEIGHT_RATIO);

	// Calculate menu position
	int menuX = static_cast<int>((width - menuWidth) / 2.0f);
	int menuY = static_cast<int>((height - menuHeight) / 2.0f);

	// Render the menu background
	SDL_SetRenderDrawColor(pRenderer, 192, 192, 192, 255);
	SDL_Rect menuRect{ menuX, menuY, menuWidth, menuHeight };
	SDL_RenderFillRect(pRenderer, &menuRect);

	// Render each option
	int optionCount = 3;
	int optionHeight = menuHeight / optionCount;
	int optionY = menuY;

	// Option 1: Start Game
	RenderMenuOption("Resume Game", menuX, optionY, menuWidth, optionHeight, textColor);
	optionY += optionHeight;

	// Option 2: Options
	RenderMenuOption("Options", menuX, optionY, menuWidth, optionHeight, textColor);
	optionY += optionHeight;

	// Option 3: Exit
	RenderMenuOption("Main Menu", menuX, optionY, menuWidth, optionHeight, textColor);
}

void Game::RenderMenuOption(const char* optionText, int x, int y, int width, int height, SDL_Color textColor) {
	SDL_Surface* surface = TTF_RenderText_Solid(pFont, optionText, textColor);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(pRenderer, surface);
	int textWidth = surface->w;
	int textHeight = surface->h;
	SDL_FreeSurface(surface);

	// Render the option text
	SDL_Rect textRect{ x + (width - textWidth) / 2, y + (height - textHeight) / 2, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &textRect);
	SDL_DestroyTexture(texture);

	// Render the option rectangle boundaries
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
	SDL_Rect rect{ x, y, width, height };
	SDL_RenderDrawRect(pRenderer, &rect);
}

void Game::Logic() {
	world.step(tick);

	if (world.bodies[anotherBranch->id]->position.y >= height - world.bodies[anotherBranch->id]->width.y / 2.0f) {
		world.bodies[anotherBranch->id]->velocity.y = -world.bodies[anotherBranch->id]->velocity.y;
	} else if (world.bodies[anotherBranch->id]->position.y <= world.bodies[anotherBranch->id]->width.y / 2.0f) {
		world.bodies[anotherBranch->id]->velocity.y = -world.bodies[anotherBranch->id]->velocity.y;
	}

	// World boundaries
	// left-right
	if (world.bodies[character->id]->position.x <= -world.bodies[character->id]->width.x) {
		ResetGame(false);
	} else if (world.bodies[character->id]->position.x >= width + world.bodies[character->id]->width.x) {
		ResetGame(false);
	}
	// top-bottom
	if (world.bodies[character->id]->position.y <= -world.bodies[character->id]->width.y) {
		ResetGame(false);
	} else if (world.bodies[character->id]->position.y >= height + world.bodies[character->id]->width.y) {
		ResetGame(false);
	}
}

void Game::RenderScene() {
	Logic();
	glClear(GL_COLOR_BUFFER_BIT);

	// Sky color
	glClearColor(135 / 255.0f, 206 / 255.0f, 235 / 255.0f, 1.0f);
	glColor3f(0, 0, 0);
	for (int i = 0; i < (int) world.bodies.size(); i++) {
		world.bodies[i]->draw();
		world.bodies[character->id]->applyTexture("../assets/sprites/character.png");
		// if (world.bodies[i]->textureId == static_cast<int>(TEXTURE::branch)) {
		// 	world.bodies[i]->applyTexture("../assets/sprites/branch.png");
		// } else if (world.bodies[i]->textureId == static_cast<int>(TEXTURE::character)) {
		// 	// world.bodies[i]->applyTexture("../assets/sprites/character.png");
		// } else if (world.bodies[i]->textureId == static_cast<int>(TEXTURE::tree)) {
		// 	// world.bodies[i]->applyTexture("../assets/sprites/tree.png");
		// }
	}
}
