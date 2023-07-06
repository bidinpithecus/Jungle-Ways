#pragma once

#include <random>
#include "physics/Body.hpp"

float randomFloat(float min, float max);
int randomInt(int min, int max);
float newVel(int x);
void loadTextureIntoBody(physics::Body& body, GLuint textureId);
