#pragma once

#include "Arbiter.hpp"
#include "Body.hpp"
#include "Math.hpp"
#include <utility>

namespace physics {

enum Axis {
	FACE_A_X,
	FACE_A_Y,
	FACE_B_X,
	FACE_B_Y
};

enum EdgeNumbers {
	NO_EDGE = 0,
	EDGE1,
	EDGE2,
	EDGE3,
	EDGE4
};

struct ClipVertex {
	Vec2 vec;
	FeaturePair featurePair;

	ClipVertex() { featurePair.value = 0; }
};

void flip(FeaturePair& featurePair);
int clipSegmentToLine(ClipVertex vOut[2], ClipVertex vIn[2], const Vec2& normal, float offset, char clipEdge);
void computeIncidentEdge(ClipVertex c[2], const Vec2& h, const Vec2& pos, const Mat22& Rot, const Vec2& normal);
int collide(Contact* contacts, Body* bodyA, Body* bodyB);

}
