#include "../../include/physics/World.hpp"

namespace physics {

using std::map;
using std::pair;

typedef pair<ArbiterKey, Arbiter> ArbPair;
typedef map<ArbiterKey, Arbiter>::iterator ArbIter;

bool World::accumulateImpulses = true;
bool World::warmStarting = false;
bool World::positionCorrection = true;

World::World() {}

void World::add(Body* body) {
	body->id = bodies.size();
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
				bi->canJump = false;
				bj->canJump = false;
				arbiters.erase(key);
			}
		}
	}
}

void World::step(float dt) {
	float invDt = dt > 0.0f ? 1.0f / dt : 0.0f;

	broadPhase();

	for (auto& body : bodies) {
		if (body->invMass == 0.0f) {
			continue;
		}

		body->velocity += dt * (gravity + body->invMass * body->force);
	}

	for (auto& arb : arbiters) {
		if (arb.second.contacts[0].position.x != arb.second.contacts[1].position.x) {
			if (arb.second.contacts[0].position.y * 1.001 >= arb.second.body1->position.y + arb.second.body1->width.y / 2.0f) {
				arb.second.body1->canJump = true;
			}
			if (arb.second.contacts[0].position.y * 1.001 >= arb.second.body2->position.y + arb.second.body2->width.y / 2.0f) {
				arb.second.body2->canJump = true;
			}
		}

		arb.second.preStep(invDt);
	}

	for (auto& joint : joints) {
		joint->preStep(invDt);
	}

	for (int i = 0; i < iterations; ++i) {
		for (auto& arb : arbiters) {
			arb.second.applyImpulse();
		}

		for (auto& joint : joints) {
			joint->applyImpulse();
		}
	}

	for (auto& body : bodies) {
		body->position += dt * body->velocity;
		body->rotation += dt * body->angularVelocity;

		body->force.set(0.0f, 0.0f);
		body->torque = 0.0f;
	}
}

}
