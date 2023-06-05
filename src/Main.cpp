#include "../include/Game.hpp"

int main(int argc, char* argv[]) {
	argc--, argv--;
	Game game = Game(1280, 720);

	return game.OnExecute();
}
