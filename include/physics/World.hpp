#pragma once

#include <vector>
#include <map>
#include "Arbiter.hpp"
#include "Joint.hpp"
#include <iostream>

namespace physics {

struct ArbiterKey;
struct Arbiter;
struct Joint;

struct World {
	std::vector<Body*> bodies;
	std::vector<Joint*> joints;
	std::map<ArbiterKey, Arbiter> arbiters;

	Vec2 gravity;
	int iterations;
	static bool accumulateImpulses;
	static bool warmStarting;
	static bool positionCorrection;

	World();
	World(Vec2 gravity, int iterations) : gravity(gravity), iterations(iterations) {}
	void add(Body* body);
	void add(Joint* joint);
	void clear();
	void step(float dt);
	void broadPhase();
};

}
