#include "../../include/physics/World.hpp"

namespace physics {

using std::map;
using std::pair;

typedef pair<ArbiterKey, Arbiter> ArbPair;
typedef map<ArbiterKey, Arbiter>::iterator ArbIter;

bool World::accumulateImpulses = true;
bool World::warmStarting = true;
bool World::positionCorrection = true;

World::World() {}

void World::add(Body* body) {
	bodies.emplace_back(body);
}

void World::add(Joint* joint) {
	joints.emplace_back(joint);
}

void World::clear() {
	bodies.clear();
	joints.clear();
	arbiters.clear();
}

void World::broadPhase() {
	// O(n^2) broad-phase
	for (int i = 0; i < (int)bodies.size(); ++i) {
		Body* bi = bodies[i];

		for (int j = i + 1; j < (int)bodies.size(); ++j) {

			Body* bj = bodies[j];

			if (bi->invMass == 0.0f && bj->invMass == 0.0f) {
				continue;
			}

			Arbiter newArb(bi, bj);
			ArbiterKey key(bi, bj);

			if (newArb.numContacts > 0) {
				ArbIter iter = arbiters.find(key);
				if (iter == arbiters.end()) {
					arbiters.insert(ArbPair(key, newArb));
				} else {
					iter->second.update(newArb.contacts, newArb.numContacts);
				}
			} else {
				arbiters.erase(key);
			}
		}
	}
}

void World::step(float dt) {
	float invDt = dt > 0.0f ? 1.0f / dt : 0.0f;

	// Determine overlapping bodies and update contact points.
	broadPhase();

	// Integrate forces.
	for (int i = 0; i < (int)bodies.size(); ++i) {
		Body* b = bodies[i];

		if (b->invMass == 0.0f) {
			continue;
		}

		b->velocity += dt * (gravity + b->invMass * b->force);
		b->angularVelocity += dt * b->invI * b->torque;
	}

	// Perform pre-steps.
	for (ArbIter arb = arbiters.begin(); arb != arbiters.end(); ++arb) {
		arb->second.preStep(invDt);
	}

	for (int i = 0; i < (int)joints.size(); ++i) {
		joints[i]->preStep(invDt);
	}

	// Perform iterations
	for (int i = 0; i < iterations; ++i) {
		for (ArbIter arb = arbiters.begin(); arb != arbiters.end(); ++arb) {
			arb->second.applyImpulse();
		}
		for (int j = 0; j < (int)joints.size(); ++j) {
			joints[j]->applyImpulse();
		}
	}

	// Integrate Velocities
	for (int i = 0; i < (int)bodies.size(); ++i) {
		Body* b = bodies[i];

		b->position += dt * b->velocity;
		b->rotation += dt * b->angularVelocity;

		b->force.set(0.0f, 0.0f);
		b->torque = 0.0f;
	}
}

}
