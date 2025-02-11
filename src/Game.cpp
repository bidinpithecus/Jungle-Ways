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

	score = 0;

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
	fruit = new physics::Body();
}

void Game::LevelDesign(bool resetCount) {
	initialBranch->set(physics::Vec2(initialTree->width.x * 2, initialTree->width.y / 20.0f), FLT_MAX);
	initialBranch->textureId = branchTextureId;

	finalBranch->set(physics::Vec2(initialBranch->width.x, initialBranch->width.y), FLT_MAX);
	finalBranch->textureId = branchTextureId;

	anotherBranch->set(physics::Vec2(initialTree->width.x * 2, initialTree->width.y / 20.0f), FLT_MAX);
	anotherBranch->textureId = branchTextureId;

	character->width.x = (initialTree->width.y / 20.0f) * 1.9f;
	character->width.y = 1.705882353F * character->width.x;
	character->set(character->width, 0.01f);
	character->textureId = characterTextureId;

	fruit->set(physics::Vec2(character->width.x, character->width.x), 0.01f);
	fruit->textureId = fruitTextureId;

	initialBranch->position.set(initialTree->position.x + initialTree->width.x / 2.0f + initialBranch->width.x / 2.0f, randomFloat(initialTree->position.y * 0.15f + character->width.y / 2.0f + initialBranch->width.y / 2.0f, initialTree->position.y * 1.875f));
	finalBranch->position.set(width - initialBranch->position.x, randomFloat(initialTree->position.y * 0.15f + character->width.y / 2.0f + finalBranch->width.y / 2.0f, initialTree->position.y * 1.875f));
	if (resetCount) {
		score = 0;
	}

	anotherBranch->velocity.y = randomInt(5 * ((score / 2.0f) + 1), 15 * ((score / 4.0f) + 1));
	fruit->position.set(finalBranch->position.x, finalBranch->position.y - finalBranch->width.y / 2.0f - fruit->width.y / 2.0f);
	world.add(fruit);

	world.add(initialBranch);
	world.add(finalBranch);

	anotherBranch->position.set(width / 2.0f, height / 2.0f);
	world.add(anotherBranch);

	character->friction = 2.0f;
	character->position.set(initialBranch->position.x, initialBranch->position.y - initialBranch->width.y / 2.0f - character->width.y / 2.0f);
	world.add(character);
}

void Game::LoadTextures() {
    characterTextureId = LoadImageIntoTexture("../assets/sprites/character.png");
    treeTextureId = LoadImageIntoTexture("../assets/sprites/tree.png");
    branchTextureId = LoadImageIntoTexture("../assets/sprites/branch.png");
	backgroundTextureId = LoadImageIntoTexture("../assets/sprites/background.png");
	fruitTextureId = LoadImageIntoTexture("../assets/sprites/fruit.png");
}

GLuint Game::LoadImageIntoTexture(const char* fileName) {
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fileName, 0);
    FIBITMAP* image = FreeImage_Load(format, fileName);
    FIBITMAP* image32bits = FreeImage_ConvertTo32Bits(image);
    int width = FreeImage_GetWidth(image32bits);
    int height = FreeImage_GetHeight(image32bits);

    // Generate a new texture ID
    GLuint textureId;
    glGenTextures(1, &textureId);

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the image data into the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(image32bits));

    // Unload the image data
    FreeImage_Unload(image);
    FreeImage_Unload(image32bits);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Return the texture ID
    return textureId;
}

void Game::ResetGame(bool resetCount) {
	world.clear();

	initialTree->set(physics::Vec2(width / 12.5f, height), FLT_MAX);
	initialTree->textureId = treeTextureId;
	initialTree->position.set(width / 12.5f, height / 2.0f);
	initialTree->friction = 0;
	world.add(initialTree);

	finalTree->set(physics::Vec2(width / 12.5f, height), FLT_MAX);
	finalTree->textureId = treeTextureId;
	finalTree->position.set(width - initialTree->position.x, height / 2.0f);
	finalTree->friction = 0;
	world.add(finalTree);

	LevelDesign(resetCount);
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

	LoadTextures();

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
		world.bodies[character->id]->velocity.x = -newVel(score);
	}
	if (keyboardStateArray[SDL_SCANCODE_D]) {
		world.bodies[character->id]->velocity.x = newVel(score);
	}
	if (keyboardStateArray[SDL_SCANCODE_SPACE] && world.bodies[character->id]->canJump) {
		world.bodies[character->id]->velocity.y = -40.0f;
	}
}

int Game::OnExecute() {
	SDL_Event event;
	if (!OnInit()) return -1;

	// FPSLimiter fps(505);
	while(isRunning) {
		while((SDL_PollEvent(&event)) != 0) {
			OnEvent(&event);
		}
		handleCharacter();
		OnLoop();
		OnRender();
		// fps.run();
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
				ResetGame(true);
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
	delete fruit;
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

	if (previousState == GAME_STATE::IN_GAME_MENU) {
		menuWidth = static_cast<int>(width * IN_GAME_MENU_WIDTH_RATIO);
		menuHeight = static_cast<int>(height * IN_GAME_MENU_HEIGHT_RATIO);
	} else if (previousState == GAME_STATE::MAIN_MENU) {
		menuWidth = static_cast<int>(width * MAIN_MENU_WIDTH_RATIO);
		menuHeight = static_cast<int>(height * MAIN_MENU_HEIGHT_RATIO);
	}

	int menuX = static_cast<int>((width - menuWidth) / 2.0f);
	int menuY = static_cast<int>((height - menuHeight) / 2.0f);

	SDL_SetRenderDrawColor(pRenderer, 192, 192, 192, 255);
	SDL_Rect menuRect{ menuX, menuY, menuWidth, menuHeight };
	SDL_RenderFillRect(pRenderer, &menuRect);

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

	// Option 1: Resume Game
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

	SDL_Rect textRect{ x + (width - textWidth) / 2, y + (height - textHeight) / 2, textWidth, textHeight };
	SDL_RenderCopy(pRenderer, texture, NULL, &textRect);
	SDL_DestroyTexture(texture);

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

	for (auto& arb : world.arbiters) {
		if ((arb.second.body1->id == character->id && arb.second.body2->id == fruit->id) || (arb.second.body2->id == character->id && arb.second.body1->id == fruit->id)) {
			score++;
			ResetGame(false);
			break;
		}
	}

	// World boundaries
	// left-right
	if (world.bodies[character->id]->position.x <= -world.bodies[character->id]->width.x) {
		ResetGame(true);
	} else if (world.bodies[character->id]->position.x >= width + world.bodies[character->id]->width.x) {
		ResetGame(true);
	}
	// top-bottom
	if (world.bodies[character->id]->position.y <= -world.bodies[character->id]->width.y) {
		ResetGame(true);
	} else if (world.bodies[character->id]->position.y >= height + world.bodies[character->id]->width.y) {
		ResetGame(true);
	}
}

void Game::RenderScene() {
	Logic();
	glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, backgroundTextureId);
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(width, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(width, height);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0.0f, height);
	glEnd();

	for (const auto& body : world.bodies) {
		glBindTexture(GL_TEXTURE_2D, body->textureId);

		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(body->position.x - body->width.x / 2.0f, body->position.y - body->width.y / 2.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(body->position.x + body->width.x / 2.0f, body->position.y - body->width.y / 2.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(body->position.x + body->width.x / 2.0f, body->position.y + body->width.y / 2.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(body->position.x - body->width.x / 2.0f, body->position.y + body->width.y / 2.0f);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glDisable(GL_TEXTURE_2D);
}

