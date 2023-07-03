#include "../include/Utils.hpp"

float randomFloat(float min, float max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(min, max);

	return dist(gen);
}
