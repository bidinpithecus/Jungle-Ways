#include "../include/Utils.hpp"
#include <random>

float randomFloat(float min, float max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(min, max);

	return dist(gen);
}

int randomInt(int min, int max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(min, max);

	return dist(gen);
}

float newVel(int x) {
	return 60 * (1 - std::exp(-0.05*x)) + 20;
}

void loadTextureIntoBody(physics::Body& body, GLuint textureId) {
    body.applyTexture(textureId);
}
