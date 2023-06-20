#pragma once

#include "Body.hpp"
#include "World.hpp"

namespace physics {

union FeaturePair {
	struct Edges {
		char inEdge1;
		char outEdge1;
		char inEdge2;
		char outEdge2;
	} e;
	int value;
};

struct Contact {
	Vec2 position;
	Vec2 normal;
	Vec2 r1, r2;

	float separation;
	float pNormal;
	float pTangent;
	float pNormalBias;
	float massNormal, massTangent;
	float bias;
	FeaturePair feature;

	Contact() : pNormal(0.0), pTangent(0.0), pNormalBias(0.0) {}
};

struct ArbiterKey {
	Body* body1;
	Body* body2;

	ArbiterKey(Body* body1, Body* body2);
};

struct Arbiter {
	enum {MAX_POINTS = 2};

	Body* body1;
	Body* body2;

	int numContacts;
	Contact contacts[MAX_POINTS];

	float friction;

	Arbiter(Body* body1, Body* body2);
	void update(Contact* contacts, int numContacts);
	void preStep(float invDt);
	void applyImpulse();
};

extern int collide(Contact* contacts, Body* body1, Body* body2);
bool operator<(const ArbiterKey& a1, const ArbiterKey& a2);

}
